// This file is part of the miniml project.
// Licensing information can be found in the LICENSE file.
// (C) Nandor Licker. All rights reserved.

#include "miniml/Heap.h"
using namespace miniml;



// -----------------------------------------------------------------------------
// Heap
// -----------------------------------------------------------------------------
Heap::Heap()
  : minorHeapSize(512 << 10 /* 4Kb */)
  , majorNodeSize(512 << 10 /* 4Kb */)
  , minorStart(nullptr)
  , minorCurrent(nullptr)
  , major(nullptr)
{
  minorStart = minorCurrent = reinterpret_cast<uint8_t*>(malloc(minorHeapSize));

  (void) major;
  (void) majorNodeSize;
}

Heap::~Heap() {
}

Value Heap::allocInt64(int64_t i) {
  return (static_cast<uint64_t>(i) << 1ull) | 1ull;
}

Value Heap::allocDouble(double v) {
  value b = allocBlock(1, kDoubleTag);
  *reinterpret_cast<double*>(val_ptr(b)) = v;
  return b;
}

Value Heap::allocBytes(size_t length) {
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
  char *ptr = reinterpret_cast<char *>(val_ptr(b));
  memset(ptr, 0, blkSize);
  ptr[blkSize - 1] = blkSize - length - 1;
  return b;
}

Value Heap::allocString(const char *str, size_t length) {
  value b = allocBytes(length);
  char *ptr = reinterpret_cast<char *>(val_ptr(b));
  memcpy(ptr, str, length);
  return b;
}

Value Heap::allocBlock(size_t n, uint8_t tag) {
  if (n >= (1ull << (64ull - 10ull))) {
    throw std::runtime_error("Block too large.");
  }

  size_t blkSize = n * sizeof(value) + sizeof(value);
  if (blkSize > minorHeapSize) {
    // Major heap allocation.
    assert(!"alloc on major heap");
  } else if (minorStart + minorHeapSize < minorCurrent + blkSize) {
    // Minor heap fill, trigger GC.
    for (const Value *n = Value::chain; n; n = n->next()) {

    }
    assert(!"minor gc");
  } else {
    void *block = reinterpret_cast<void*>(minorCurrent);
    minorCurrent += blkSize;

    if (block == nullptr) {
      throw std::runtime_error("Allocation failed.");
    }

    *reinterpret_cast<uint64_t *>(block) = (n << 10) | tag;
    for (size_t i = 0; i < n; ++i) {
      *(reinterpret_cast<value *>(block) + i + 1) = 1ull;
    }
    return reinterpret_cast<value>(block) + sizeof(uint64_t);
  }
}

Value Heap::allocCustom(CustomOperations *op, size_t size) {
  const size_t words = 1 + (size + sizeof(value) - 1) / sizeof(value);
  value b = allocBlock(words, kCustomTag);
  val_field(b, 0) = reinterpret_cast<value>(op);
  return b;
}
