// This file is part of the miniml project.
// Licensing information can be found in the LICENSE file.
// (C) Nandor Licker. All rights reserved.

#pragma once

#include "miniml/Value.h"

namespace miniml {

// Heap managing memory.
class Heap {
 public:
  /// Initializes the heap.
  Heap();
  /// Destroys the heap.
  ~Heap();

  // Allocates values on the heap.
  Value allocInt64(int64_t i);
  Value allocDouble(double v);
  Value allocBytes(size_t length);
  Value allocString(const char *str, size_t length);
  Value allocBlock(size_t n, uint8_t tag);
  Value allocCustom(CustomOperations *ops, size_t size);

 private:
  /// Size of the minor heap.
  size_t minorHeapSize;
  /// Size of a major node.
  size_t majorNodeSize;

  /// Start address of the minor heap.
  uint8_t *minorStart;
  /// Last allocated block in the minor heap.
  uint8_t *minorCurrent;

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
  Major *major;
};

}
