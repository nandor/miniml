// This file is part of the miniml project.
// Licensing information can be found in the LICENSE file.
// (C) Nandor Licker. All rights reserved.

#pragma once

#include "miniml/Value.h"

namespace miniml {

// Heap managing memory.
class Heap {
 public:

  // Allocates values on the heap.
  value allocInt64(int64_t i);
  value allocDouble(double v);
  value allocBytes(size_t length);
  value allocString(const char *str, size_t length);
  value allocBlock(size_t n, uint8_t tag);
  value allocCustom(CustomOperations *ops, size_t size);

 private:
  /// Start address of the minor heap.
  uint8_t *minorStart;
  /// Last allocated block in the minor heap.
  uint8_t *minorCurrent;

  /// Size of the minor heap.
  size_t minoHeapSize;
  /// Size of a major node.
  size_t majorNodeSize;

  // Major heap node.
  struct Major {
    /// Next major node.
    Major *next;
    /// Start address of a major node.
    uint8_t *start;
    /// Current pointer in the major node.
    uint8_t *current;
  };

  // First heap node.
  Major *first;
};

}
