// This file is part of the miniml project.
// Licensing information can be found in the LICENSE file.
// (C) Nandor Licker. All rights reserved.

#pragma once

#include <unordered_map>
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
  Value allocBytes(size_t length);
  Value allocString(const char *str, size_t length);
  Value allocBlock(size_t n, uint8_t tag);
  Value allocCustom(CustomOperations *op, size_t size);
  Value allocAtom(uint8_t id);

  // Custom value operations.
  void registerOperations(CustomOperations *value);
  CustomOperations *getOperations(const std::string &name);

  // Executes a bytecode file.
  void run(BytecodeFile &file);

 private:
  /// Interpreter is a friend.
  friend class Heap;
  /// Memory Manager.
  Heap heap_;
  /// List of atoms.
  Value atom_[256];
  /// List of custom values.
  std::unordered_map<std::string, CustomOperations *> custom_;
};

} // namespace miniml
