// This file is part of the miniml project.
// Licensing information can be found in the LICENSE file.
// (C) Nandor Licker. All rights reserved.

#pragma once

#include <vector>

#include "miniml/Value.h"



namespace miniml {
class Heap;
class BytecodeFile;

/// Context providing access to the environment.
class Context {
 public:

  // Allocates values on the heap.
  Value allocInt64(int64_t i);
  Value allocDouble(double v);
  Value allocString(const char *str, size_t length);
  Value allocBlock(size_t n, uint8_t tag);

  // Interprets a bytecode file.
  void run(BytecodeFile &file);

  void runACC(uint32_t n);
  void runPUSH();
  void runPUSHACC(uint32_t n);
  void runPOP(uint32_t n);
  void runASSIGN();
  void runENVACC();
  void runGETFIELD();
  void runPUSH_RETADDR();
  void runAPPLY(uint32_t args);
  void runRESTART();
  void runGRAB();
  void runCLOSURE();
  void runCLOSUREREC();
  void runPUSHOFFSETCLOSURE();
  void runGETGLOBAL(uint32_t n);
  void runPUSHGETGLOBAL(uint32_t n);
  void runGETGLOBALFIELD();
  void runPUSHGETGLOBALFIELD();
  void runSETGLOBAL(uint32_t n);
  void runATOM0();
  void runATOM();
  void runPUSHATOM0();
  void runPUSHATOM();
  void runMAKEBLOCK(uint32_t n);
  void runBRANCH(int32_t ofs);
  void runBRANCHIF(int32_t ofs);
  void runBRANCHIFNOT(int32_t ofs);
  void runSWITCH();
  void runPUSHTRAP();
  void runPOPTRAP();
  void runCHECK_SIGNALS();
  void runPUSHCONST0();
  void runCCALL(uint32_t n);

 private:
  /// Memory Manager.
  Heap *heap;
  /// Code being executed.
  const uint32_t *code;
  /// Operand stack.
  std::vector<Value> stack;
  /// Program counter.
  uint64_t PC;
  /// Accumulator.
  Value A;
  /// Stack pointer of the highest exception handler.
  uint64_t trapSP;
  /// Number of extra arguments to a function.
  uint64_t extraArgs;
  /// Environment.
  Value env;
  /// Global state.
  Value global;
  /// Primitive names.
  std::vector<std::string> prim_;
};

} // namespace miniml
