// This file is part of the miniml project.
// Licensing information can be found in the LICENSE file.
// (C) Nandor Licker. All rights reserved.

#pragma once

#include <cassert>
#include <cstring>
#include <ostream>

#include <iostream>

#include "miniml/Stream.h"

namespace miniml {
class Context;
class Heap;



/// Well-known tags.
static const uint8_t kClosureTag     = 247;
static const uint8_t kObjectTag      = 248;
static const uint8_t kInfixTag       = 249;
static const uint8_t kNoScanTag      = 251;
static const uint8_t kStringTag      = 252;
static const uint8_t kDoubleTag      = 253;
static const uint8_t kDoubleArrayTag = 254;
static const uint8_t kCustomTag      = 255;



/// All values are encoded in 64 bits.
typedef uint64_t value;



/// Table of custom operations.
struct CustomOperations {
  const char *identifier;
  void     (*finalize)    (Context &ctx, value);
  int      (*compare)     (Context &ctx, value, value);
  uint64_t (*hash)        (Context &ctx, value, value);
  void     (*serialize)   (Context &ctx, value, StreamWriter &stream);
  value    (*deserialize) (Context &ctx, StreamReader &stream);
  void     (*print)       (Context &ctx, value, std::ostream &os);
  int      (*compare_ext) (Context &ctx, value, value);
};



/// Creates a value from an int64.
inline value val_int64(int64_t val) {
  return (val << 1ll) + 1ll;
}
/// Checks if a value is an integer.
inline bool val_is_int64(value val) {
  return (val & 0x1) == 0x1;
}
/// Checks if a value is a block.
inline bool val_is_block(value val) {
  return (val & 0x1) == 0x0;
}
/// Returns the block header.
inline uint64_t &val_header(value val) {
  assert((val & 1) == 0 && "Value is not a block.");
  return *(reinterpret_cast<uint64_t *>(val) - 1);
}
/// Return the block tag.
inline uint8_t val_tag(value val) {
  return val_header(val) & 0xFF;
}
/// Returns the size of the block (in words).
inline uint64_t val_size(value val) {
  return val_header(val) >> 10;
}
/// Returns a pointer to the first field.
inline value *val_ptr(value val) {
  assert((val & 1) == 0 && "Value is not a block.");
  return reinterpret_cast<value *>(val);
}
/// Returns a pointer to a field.
inline value &val_field(value val, size_t n) {
  assert((val_size(val) >= n) && "Index out of bounds");
  return *(val_ptr(val) + n);
}
/// Returns the pointer to a custom value.
template<typename T>
inline T* val_to_custom(value val) {
  assert(val_tag(val) == kCustomTag && "Value is not custom.");
  return reinterpret_cast<T*>(&val_field(val, 1));
}
/// Returns a pointer to the ops.
inline CustomOperations *val_ops(value val) {
  assert(val_tag(val) == kCustomTag && "Value is not custom.");
  return reinterpret_cast<CustomOperations*>(val_field(val, 0));
}
/// Returns the code value of a closure.
inline uint64_t &val_code(value val) {
  assert(val_tag(val) == kClosureTag && "Value is not a closure.");
  return reinterpret_cast<uint64_t&>(val_field(val, 0));
}
/// Extracts the int value.
inline int64_t val_to_int64(value val) {
  assert((val & 1) == 1 && "Value is not an integer.");
  return static_cast<int64_t>(val) >> 1ll;
}
/// Extracts the double value.
inline double val_to_double(value val) {
  assert(val_tag(val) == kDoubleTag && "Value is not a double.");
  return *reinterpret_cast<double *>(val_ptr(val));
}
/// Extracts the string value.
inline char *val_to_string(value val) {
  assert(val_tag(val) == kStringTag && "Value is not a string.");
  return reinterpret_cast<char *>(val_ptr(val));
}
/// Computes the length of a string.
inline size_t val_strlen(value val) {
  assert(val_tag(val) == kStringTag && "Value is not a string.");
  const auto data = reinterpret_cast<const char *>(val_ptr(val));
  const size_t blkSize = val_size(val) * sizeof(value);
  return blkSize - data[blkSize - 1] - 1;
}


/// Wrapper around a block/integer.
class Value final {
 public:
  /// Default value: integer 0.
  Value()
    : value_(val_int64(0ll))
  {
  }

  /// Wraps the heap-allocated value.
  Value(value value)
    : value_(value)
  {
    link();
  }

  /// Copies a heap-allocated value.
  Value(const Value &value)
    : value_(value.value_)
  {
    link();
  }

  /// Frees the local reference.
  ~Value() {
    unlink();
  }

  /// Creates a copy of the reference.
  Value &operator=(const Value &value) {
    unlink();
    value_ = value.value_;
    link();
    return *this;
  }

  /// Fetches the underlying value.
  operator value () {
    return value_;
  }

  /// Checks if the value is an integer or a block.
  inline bool isInt64() const {
    return value_ & 1;
  }
  inline bool isBlock() const {
    return !(value_ & 1);
  }
  /// Checks if the value is a specific object.
  inline bool isClosure() const { return tag() == kClosureTag; }
  inline bool isObject() const { return tag() == kObjectTag; }
  inline bool isDouble() const { return tag() == kDoubleTag; }
  inline bool isString() const { return tag() == kStringTag; }
  inline bool isCustom() const { return tag() == kCustomTag; }

  /// Returns the header of a block.
  inline uint64_t header() const {
    return val_header(value_);
  }
  /// Returns the tag, if the value is an integer.
  inline uint8_t tag() const {
    return val_tag(value_);
  }
  /// Returns the size of the block (number of words).
  inline size_t size() const {
    return val_size(value_);
  }

  /// Returns a pointer to the first field.
  inline const value *ptr() const {
    return val_ptr(value_);
  }
  inline value *ptr() {
    return val_ptr(value_);
  }

  /// Returns some special attributes.
  inline const CustomOperations *getOps() const {
    return val_ops(value_);
  }
  inline uint64_t getCode() const {
    return val_code(value_);
  }

  /// Decodes the value.
  inline int64_t getInt64() const {
    return val_to_int64(value_);
  }
  inline double getDouble() const {
    return val_to_double(value_);
  }
  inline const char *getString() const {
    return val_to_string(value_);
  }

  /// Returns the length of a string.
  size_t strlen() const {
    return val_strlen(value_);
  }

  /// Sets the code of a closure.
  inline void setCode(uint64_t code) {
    val_code(value_) = code;
  }

  /// Sets a field of the block.
  inline void setField(size_t n, Value value) {
    val_field(value_, n) = value.value_;
  }

  /// Reads a field of the block.
  inline Value getField(size_t n) {
    return val_field(value_, n);
  }

  /// First node in the value chain.
  static Value *chain;

 private:
  /// Links the value into the chain.
  void link();
  /// Unlinks the value from the chain.
  void unlink();

 private:
  /// Wrapped value.
  value value_;
  /// Previous node in the value chain.
  Value *prev_;
  /// Next node in the value chain.
  Value *next_;
};



/// Common constants.
static Value kUnit(1ull);
static Value kTrue(3ull);
static value kFalse(1ull);



/// Deserializes a value, reading it from a stream.
Value getValue(Context &ctx, StreamReader &stream);

/// Serializes a value, writing it to a stream.
void putValue(Context &ctx, Value value, StreamWriter &stream);

/// Prints a value's text representation.
void printValue(Context &ctx, Value value, std::ostream &os);

}
