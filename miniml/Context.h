// This file is part of the miniml project.
// Licensing information can be found in the LICENSE file.
// (C) Nandor Licker. All rights reserved.

#pragma once

#include <vector>

#include "miniml/Value.h"
#include "miniml/Heap.h"



namespace miniml {
class Heap;
class BytecodeFile;

/// Context providing access to the environment.
class Context {
 public:
  // Creates a new context.
  Context();
  // Deletes the context.
  virtual ~Context();

  // Allocates values on the heap.
  Value allocInt64(int64_t i);
  Value allocDouble(double v);
  Value allocString(const char *str, size_t length);
  Value allocBlock(size_t n, uint8_t tag);

  // Executes a bytecode file.
  void run(BytecodeFile &file);

 private:
  /// Memory Manager.
  Heap heap;
};

} // namespace miniml
