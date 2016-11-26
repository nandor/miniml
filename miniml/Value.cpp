// This file is part of the miniml project.
// Licensing information can be found in the LICENSE file.
// (C) Nandor Licker. All rights reserved.

#include "miniml/Context.h"
#include "miniml/Value.h"
using namespace miniml;


/// Magic value of serializes blocks.
static const uint32_t kBlockMagic = 0x8495A6BE;



// -----------------------------------------------------------------------------
// getValue
// -----------------------------------------------------------------------------
static Value getValueImpl(Context &ctx, StreamReader &stream) {
  switch (auto code = stream.getUInt8()) {
  case 0x00: return ctx.allocInt64(stream.getInt8());
  case 0x01: return ctx.allocInt64(stream.getInt16be());
  case 0x02: return ctx.allocInt64(stream.getInt32be());
  case 0x03: return ctx.allocInt64(stream.getInt64be());
  case 0x04: return ctx.allocInt64(stream.getUInt8());
  case 0x05: return ctx.allocInt64(stream.getUInt16be());
  case 0x06: return ctx.allocInt64(stream.getUInt32be());
  case 0x08: {
    // Object with 32-bit header.
    uint32_t header = stream.getUInt32be();
    size_t size = header >> 10;
    uint8_t tag = header & 0xFF;
    Value value = ctx.allocBlock(size, tag);
    for (size_t i = 0; i < size; ++i) {
      value.setField(i, getValueImpl(ctx, stream));
    }
    return value;
  }
  case 0x09: {
    // String with 8-bit header.
    size_t length = stream.getUInt8();
    return ctx.allocString(stream.getString(length), length);
  }
  case 0x0A: {
    // String with 32-bit header.
    size_t length = stream.getUInt32be();
    return ctx.allocString(stream.getString(length), length);
  }
  case 0x0B: case 0x0C: {
    return ctx.allocDouble(stream.getDouble());
  }
  case 0x0D: case 0x0E:{
    // Sequence of doubles with 8-bit header.
    size_t length = stream.getUInt8();
    Value value = ctx.allocBlock(length, kDoubleArrayTag);
    for (size_t i = 0; i < length; ++i) {
      value.setField(i, ctx.allocDouble(stream.getDouble()));
    }
    return value;
  }
  case 0x07: case 0x0F: {
    // Sequence of doubles with 8-bit header.
    size_t length = stream.getUInt32be();
    Value value = ctx.allocBlock(length, kDoubleArrayTag);
    for (size_t i = 0; i < length; ++i) {
      value.setField(i, ctx.allocDouble(stream.getDouble()));
    }
    return value;
  }
  case 0x10:
  case 0x11: {
    throw std::runtime_error("Not implemented.");
  }
  case 0x12: {
    return ctx.getOperations(stream.getString())->deserialize(ctx, stream);
  }
  case 0x13: {
    // Object with 64-bit header.
    uint64_t header = stream.getUInt64be();
    size_t size = header >> 10;
    uint8_t tag = header & 0xFF;
    Value value = ctx.allocBlock(size, tag);
    for (size_t i = 0; i < size; ++i) {
      value.setField(i, getValueImpl(ctx, stream));
    }
    return value;
  }
  case 0x20 ... 0x3F: {
    // Tiny string.
    size_t length = code & 0x1F;
    return ctx.allocString(stream.getString(length), length);
  }
  case 0x40 ... 0x7F: {
    // Tiny int.
    return ctx.allocInt64(code & 0x3F);
  }
  case 0x80 ... 0xFF: {
    // Tiny block.
    size_t size = (code >> 4) & 0x7;
    uint8_t tag = code & 0xF;
    Value value = ctx.allocBlock(size, tag);
    for (size_t i = 0; i < size; ++i) {
      value.setField(i, getValueImpl(ctx, stream));
    }
    return value;
  }
  default:
    throw std::runtime_error("Invalid value code: " + std::to_string(code));
  }
}

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

  uint32_t objCount = stream.getUInt32be();
  uint32_t size32 = stream.getUInt32be();
  uint32_t size64 = stream.getUInt32be();

  return getValueImpl(ctx, stream);
}



// -----------------------------------------------------------------------------
// putValue
// -----------------------------------------------------------------------------
void miniml::putValue(Context &ctx, Value value, StreamWriter &stream) {
}



// -----------------------------------------------------------------------------
// printValue
// -----------------------------------------------------------------------------
static void printValueImpl(Value value, std::ostream &os, size_t indent) {
  for (size_t i = 0; i < indent; ++i) {
    os << "  ";
  }
  if (value.isInt64()) {
    os << value.getInt64();
    return;
  }
  if (value.isDouble()) {
    os << value.getDouble();
    return;
  }
  if (value.isString()) {
    os << '"';
    const char *str = value.getString();
    for (size_t i = 0; i < value.strlen(); ++i) {
      if (isprint(str[i])) {
        os << str[i];
      } else {
        os << ".";
      }
    }
    os << '"';
    return;
  }
  if (value.isBlock()) {
    os << "(" << static_cast<int>(value.tag()) << ") {\n";
    for (size_t i = 0; i < value.size(); ++i) {
      printValueImpl(value.getField(i), os, indent + 1);
      os << "\n";
    }
    for (size_t i = 0; i < indent; ++i) {
      os << "  ";
    }
    os << "}";
  }
}

void miniml::printValue(Context &ctx, Value value, std::ostream &os) {
  printValueImpl(value, os, 0);
  os << "\n";
}
