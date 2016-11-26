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
static const uint8_t kInfixTag       = 249;
static const uint8_t kStringTag      = 252;
static const uint8_t kDoubleTag      = 253;
static const uint8_t kDoubleArrayTag = 254;

/// All values are encoded in 64 bits.
typedef uint64_t value;

/// Wrapper around a block/integer.
class Value {
 public:
  /// Default value: integer 0.
  Value()
    : value_(1ull)
  {
  }

  /// Wraps the heap-allocated value.
  Value(value value)
    : value_(value)
  {
  }

  /// Copies a heap-allocated value.
  Value(const Value &value)
    : value_(value.value_)
  {
  }

  /// Frees the local reference.
  ~Value() {
  }

  /// Creates a copy of the reference.
  Value &operator=(const Value &value) {
    value_ = value.value_;
    return *this;
  }

  /// Checks if the value is of a specific type.
  inline bool isInt64() const {
    return value_ & 1;
  }
  inline bool isDouble() const {
    return !(value_ & 1) && tag() == kDoubleTag;
  }
  inline bool isString() const {
    return !(value_ & 1) && tag() == kStringTag;
  }
  inline bool isBlock() const {
    return !(value_ & 1);
  }

  /// Returns the header of a block.
  inline uint64_t header() const {
    assert((value_ & 1) == 0 && "Value is an integer.");
    return *(reinterpret_cast<const uint64_t *>(value_));
  }

  /// Returns the tag, if the value is an integer.
  inline uint8_t tag() const {
    assert((value_ & 1) == 0 && "Value is not a block.");
    return header() & 0xFF;
  }

  /// Returns the size of the block (number of words).
  inline size_t size() const {
    return header() >> 10;
  }

  /// Returns a pointer to the first field.
  inline value *ptr() const {
    assert((value_ & 1) == 0 && "Value is not a block.");
    return reinterpret_cast<value *>(value_) + 1;
  }

  /// Decodes the value.
  inline int64_t getInt64() const {
    assert((value_ & 1) == 1 && "Value is not an integer.");
    return static_cast<int64_t>(value_) >> 1ll;
  }
  inline double getDouble() const {
    assert(tag() == kDoubleTag && "Value is not a double.");
    return static_cast<double>(*ptr());
  }
  inline const char *getString() const {
    assert(tag() == kStringTag && "Value is not a string.");
    return reinterpret_cast<const char *>(ptr());
  }

  /// Returns the length of a string.
  size_t strlen() const {
    assert(tag() == kStringTag && "Value is not a string.");
    const auto data = reinterpret_cast<const char *>(ptr());
    const size_t blkSize = size() * sizeof(value);
    return blkSize - data[blkSize - 1] - 1;
  }

  /// Sets a field of the block.
  inline void setField(size_t n, Value value) {
    assert((size() >= n) && "Index out of bounds");
    *(ptr() + n) = value.value_;
  }

  /// Reads a field of the block.
  inline Value getField(size_t n) {
    assert((size() >= n) && "Index out of bounds");
    return *(ptr() + n);
  }

 private:
  /// Wrapped value.
  value value_;
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
