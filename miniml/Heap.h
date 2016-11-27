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
  value allocString(const char *str, size_t length);
  value allocBlock(size_t n, uint8_t tag);
  value allocCustom(CustomOperations *ops, size_t size);

 private:
  // Heap node.
  struct Node {
    Node *next;
    void *mem;
  };

  // First heap node.
  Node *first;
};

}
