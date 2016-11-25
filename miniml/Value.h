// This file is part of the miniml project.
// Licensing information can be found in the LICENSE file.
// (C) Nandor Licker. All rights reserved.

#pragma once

#include <cstring>
#include <ostream>

#include <iostream>

#include "miniml/Stream.h"

namespace miniml {
class Context;

/// Well-known tags.
static const uint8_t kDoubleTag = 0xFC;
static const uint8_t kDoubleArrayTag = 0xFD;


/// Wrapper around a block/integer.
class Value {
 public:
  Value()
    : type_(0), i_(0) {
  }
  Value(int64_t i)
    : type_(0), i_(i) {
  }
  Value(double d)
    : type_(1), d_(d) {
  }
  Value(const char *str, size_t length)
    : type_(2), str_(strdup(str)), length_(length) {
  }
  Value(size_t size, uint8_t tag)
    : type_(3), size_(size), tag_(tag), values_(new Value[size]) {
  }

  Value(const Value &value) {
    copy(value);
  }

  ~Value() {
    clear();
  }

  Value &operator=(const Value &value) {
    clear();
    copy(value);
    return *this;
  }

  void copy(const Value &value) {
    type_ = value.type_;
    if (type_ == 0) {
      i_ = value.i_;
    } else if (type_ == 1) {
      d_ = value.d_;
    } else if (type_ == 2) {
      str_ = strdup(value.str_);
      length_ = value.length_;
    } else {
      size_ = value.size_;
      tag_ = value.tag_;
      values_ = new Value[size_];
      for (size_t i = 0; i < size_; ++i) {
        values_[i] = value.values_[i];
      }
    }
  }

  void clear() {
    if (type_ == 2) {
      free(str_);
    }
    if (type_ == 3) {
      delete[] values_;
    }
  }

  /// Sets a field of the block.
  void setField(size_t n, Value value);
  /// Reads a field of the block.
  Value getField(size_t n);

  /// Checks if the value is of a specific type.
  bool isInt64() const { return type_ == 0; }
  bool isDouble() const { return type_ == 1; }
  bool isString() const { return type_ == 2; }
  bool isBlock() const { return type_ == 3; }

  /// Decodes the value.
  int64_t getInt64() const { return i_; }
  double getDouble() const { return d_; }
  const char *getString() const { return str_; }
  size_t getSize() const { return isString() ? length_ : size_; }
  uint8_t getTag() const { return tag_; }

  int type_;
  union {
    int64_t i_;
    double d_;
    struct {
      char *str_;
      size_t length_;
    };
    struct {
      size_t size_;
      uint8_t tag_;
      Value *values_;
    };
  };
};


/// Deserializes a value, reading it from a stream.
Value getValue(Context &ctx, StreamReader &stream);

/// Serializes a value, writing it to a stream.
void putValue(Context &ctx, StreamWriter &stream);

/// Prints a value's text representation.
void printValue(Context &ctx, Value value, std::ostream &os);

}
