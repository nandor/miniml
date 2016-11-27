// This file is part of the miniml project.
// Licensing information can be found in the LICENSE file.
// (C) Nandor Licker. All rights reserved.

#include "miniml/Heap.h"
using namespace miniml;



// -----------------------------------------------------------------------------
// Heap
// -----------------------------------------------------------------------------
value Heap::allocInt64(int64_t i) {
  return (static_cast<uint64_t>(i) << 1ull) | 1ull;
}

value Heap::allocDouble(double v) {
  value b = allocBlock(1, kDoubleTag);
  *reinterpret_cast<double*>(reinterpret_cast<value *>(b) + 1) = v;
  printf("D: %p\n", reinterpret_cast<void*>(b));
  return b;
}

value Heap::allocString(const char *str, size_t length) {
  // The length of all blocks must be a multiple of the word size, i.e. 8 bytes.
  // Thus, the memory allocated to strings must be a multiple of the word size.
  // The string is padded by a number of bytes and the last byte represents the
  // number of padding bytes added.
  //
  // length = 4, blkSize = 8, pad = 3
  //
  // 0   1   2   3   4   5   6   7
  // 'a' 'b' 'c' 'd' \0  \0  \0  \3
  const size_t size = (length + sizeof(value)) / sizeof(value);
  const size_t blkSize = size * sizeof(value);
  value b = allocBlock(size, kStringTag);
  char *ptr = reinterpret_cast<char *>(reinterpret_cast<value *>(b) + 1);
  memcpy(ptr, str, length);
  for (size_t i = length; i < blkSize - 1; ++i) {
    ptr[i] = 0;
  }
  ptr[blkSize - 1] = blkSize - length - 1;
  return b;
}

value Heap::allocBlock(size_t n, uint8_t tag) {
  if (n >= (1ull << (64ull - 10ull))) {
    throw std::runtime_error("Block too large.");
  }

  void *block = malloc(n * sizeof(value) + sizeof(value));
  *reinterpret_cast<uint64_t *>(block) = (n << 10) | tag;
  for (size_t i = 0; i < n; ++i) {
    *(reinterpret_cast<value *>(block) + i + 1) = 1ull;
  }
  return reinterpret_cast<value>(block);
}

value Heap::allocCustom(CustomOperations *op, size_t size) {
  const size_t words = 1 + (size + sizeof(value) - 1) / sizeof(value);
  value b = allocBlock(words, kCustomTag);
  val_field(b, 0) = reinterpret_cast<value>(op);
  return b;
}
