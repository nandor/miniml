// This file is part of the miniml project.
// Licensing information can be found in the LICENSE file.
// (C) Nandor Licker. All rights reserved.

#pragma once

#include "miniml/Value.h"



namespace miniml {
class Heap;

/// Context providing access to the environment.
class Context {
 public:

  Value allocInt64(int64_t i);
  Value allocDouble(double v);
  Value allocString(const char *str, size_t length);
  Value allocBlock(size_t n, uint8_t tag);

 private:
  /// Memory Manager.
  Heap *heap;
};

} // namespace miniml
