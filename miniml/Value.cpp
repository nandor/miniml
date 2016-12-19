// This file is part of the miniml project.
// Licensing information can be found in the LICENSE file.
// (C) Nandor Licker. All rights reserved.

#include "miniml/Context.h"
#include "miniml/Value.h"
using namespace miniml;


/// Magic value of serializes blocks.
static const uint32_t kBlockMagic = 0x8495A6BE;

/// First node of the value chain.
Value *Value::chain = nullptr;



// -----------------------------------------------------------------------------
// Value
// -----------------------------------------------------------------------------
void Value::link() {
  if (value_ & 0x1) {
    return;
  }
  prev_ = nullptr;
  next_ = chain;
  if (chain) {
    chain->prev_ = this;
  }
  chain = this;
}

void Value::unlink() {
  if (value_ & 0x1) {
    return;
  }
  if (this == chain) {
    chain = this->next_;
  }
  if (prev_) {
    prev_->next_ = next_;
  }
  if (next_) {
    next_->prev_ = prev_;
  }
}



// -----------------------------------------------------------------------------
// getValue
// -----------------------------------------------------------------------------
class ValueReader {
 public:
  ValueReader(Context &ctx, StreamReader &stream, size_t count)
    : ctx_(ctx)
    , stream_(stream)
    , objects_(count, 0ll)
    , index_(0)
  {
  }

  Value read() {
    switch (auto code = stream_.getUInt8()) {
    case 0x00: return ctx_.allocInt64(stream_.getInt8());
    case 0x01: return ctx_.allocInt64(stream_.getInt16be());
    case 0x02: return ctx_.allocInt64(stream_.getInt32be());
    case 0x03: return ctx_.allocInt64(stream_.getInt64be());
    case 0x04: return objects_[index_ - stream_.getUInt8()];
    case 0x05: return objects_[index_ - stream_.getUInt16be()];
    case 0x06: return objects_[index_ - stream_.getUInt32be()];
    case 0x08: {
      // Object with 32-bit header.
      uint32_t header = stream_.getUInt32be();
      size_t size = header >> 10;
      uint8_t tag = header & 0xFF;
      Value val = ctx_.allocBlock(size, tag);
      objects_[index_++] = val;
      for (size_t i = 0; i < size; ++i) {
        val.setField(i, read());
      }
      return val;
    }
    case 0x09: {
      // String with 8-bit header.
      size_t length = stream_.getUInt8();
      Value val = ctx_.allocString(stream_.getString(length), length);
      objects_[index_++] = val;
      return val;
    }
    case 0x0A: {
      // String with 32-bit header.
      size_t length = stream_.getUInt32be();
      Value val = ctx_.allocString(stream_.getString(length), length);
      objects_[index_++] = val;
      return val;
    }
    case 0x0B: case 0x0C: {
      Value val = ctx_.allocDouble(stream_.getDouble());
      objects_[index_++] = val;
      return val;
    }
    case 0x0D: case 0x0E:{
      // Sequence of doubles with 8-bit header.
      size_t length = stream_.getUInt8();
      Value val = ctx_.allocBlock(length, kDoubleArrayTag);
      objects_[index_++] = val;
      for (size_t i = 0; i < length; ++i) {
        val.setField(i, dbl_to_val(stream_.getDouble()));
      }
      return val;
    }
    case 0x07: case 0x0F: {
      // Sequence of doubles with 32-bit header.
      size_t length = stream_.getUInt32be();
      Value val = ctx_.allocBlock(length, kDoubleArrayTag);
      objects_[index_++] = val;
      for (size_t i = 0; i < length; ++i) {
        val.setField(i, dbl_to_val(stream_.getDouble()));
      }
      return val;
    }
    case 0x10:
    case 0x11: {
      throw std::runtime_error("Not implemented.");
    }
    case 0x12: {
      auto name = stream_.getString();
      if (auto ops = ctx_.getOperations(name)) {
        Value val = ops->deserialize(ctx_, stream_);
        objects_[index_++] = val;
        return val;
      } else {
        throw std::runtime_error("Unimplemented custom '" + name + "'");
      }
    }
    case 0x13: {
      // Object with 64-bit header.
      uint64_t header = stream_.getUInt64be();
      size_t size = header >> 10;
      uint8_t tag = header & 0xFF;
      Value val = ctx_.allocBlock(size, tag);
      objects_[index_++] = val;
      for (size_t i = 0; i < size; ++i) {
        val.setField(i, read());
      }
      return val;
    }
    case 0x14: return objects_[index_ - stream_.getUInt64be()];
    case 0x20 ... 0x3F: {
      // Tiny string.
      size_t length = code & 0x1F;
      Value val = ctx_.allocString(stream_.getString(length), length);
      objects_[index_++] = val;
      return val;
    }
    case 0x40 ... 0x7F: {
      // Tiny int.
      return ctx_.allocInt64(code & 0x3F);
    }
    case 0x80 ... 0xFF: {
      // Tiny block.
      size_t size = (code >> 4) & 0x7;
      Value val = ctx_.allocBlock(size, code & 0xF);
      objects_[index_++] = val;
      for (size_t i = 0; i < size; ++i) {
        val.setField(i, read());
      }
      return val;
    }
    default:
      throw std::runtime_error("Invalid value code: " + std::to_string(code));
    }
  }

 private:
  /// ML context.
  Context &ctx_;
  /// Stream we are reading from.
  StreamReader &stream_;
  /// Object cache.
  std::vector<Value> objects_;
  /// Current object index.
  size_t index_;
};

Value miniml::getValue(Context &ctx, StreamReader &stream) {
  // Reads the header.
  uint32_t magic = stream.getUInt32be();
  if (magic != kBlockMagic) {
    throw std::runtime_error("Invalid block magic");
  }
  // Verify length, if possible.
  uint32_t length = stream.getUInt32be();
  if (auto streamLength = stream.length()) {
    if (length + 5 * sizeof(uint32_t) != streamLength) {
      throw std::runtime_error("Invalid block length");
    }
  }

  // Read/skip the header.
  uint32_t objCount = stream.getUInt32be();
  stream.getUInt32be();
  stream.getUInt32be();

  // Read the values.
  return ValueReader(ctx, stream, objCount).read();
}



// -----------------------------------------------------------------------------
// putValue
// -----------------------------------------------------------------------------
void miniml::putValue(Context &ctx, Value value, StreamWriter &stream) {
  (void) ctx;
  (void) value;
  (void) stream;
}



// -----------------------------------------------------------------------------
// printValue
// -----------------------------------------------------------------------------
void miniml::printValue(Context &ctx, Value val, std::ostream &os) {
  /// Helper to recursively print values.
  std::function<void(Value, size_t)> print = [&](Value val, size_t indent) {
    if (val.isInt64()) {
      os << val.getInt64();
      return;
    }
    if (val.isDouble()) {
      os << val.getDouble();
      return;
    }
    if (val.isString()) {
      os << '"';
      const char *str = val.getString();
      for (size_t i = 0; i < val.strlen(); ++i) {
        if (isprint(str[i])) {
          os << str[i];
        } else {
          os << ".";
        }
      }
      os << '"';
      return;
    }
    if (val.isCustom()) {
      auto ops = val.getOps();
      os << "<custom:";
      if (ops->print) {
        ops->print(ctx, val, os);
      }
      os << ">";
      return;
    }
    if (val.isClosure()) {
      os << "<closure:";
      os << static_cast<value>(val.getField(0));
      os << ">";
      return;
    }
    if (val.isBlock()) {
      os << "(" << static_cast<int>(val.tag()) << ") {\n";
      for (size_t i = 0; i < val.size(); ++i) {
        for (size_t j = 0; j <= indent; ++j) {
          os << "  ";
        }
        os << i << ": ";
        print(val.getField(i), indent + 1);
        os << "\n";
      }
      for (size_t i = 0; i < indent; ++i) {
        os << "  ";
      }
      os << "}";
    }
  };

  /// Print value, recursing into blocks.
  print(val, 0);
  os << "\n";
}
