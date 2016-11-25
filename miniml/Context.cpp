// This file is part of the miniml project.
// Licensing information can be found in the LICENSE file.
// (C) Nandor Licker. All rights reserved.

#include "miniml/Context.h"
using namespace miniml;



// -----------------------------------------------------------------------------
// Context
// -----------------------------------------------------------------------------
Value Context::allocInt64(int64_t i) {
  return Value(i);
}

Value Context::allocDouble(double v) {
  return Value(v);
}

Value Context::allocString(const char *str, size_t length) {
  return Value(str, length);
}

Value Context::allocBlock(size_t n, uint8_t tag) {
  return Value(n, tag);
}

