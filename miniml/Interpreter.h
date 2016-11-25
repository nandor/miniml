// This file is part of the miniml project.
// Licensing information can be found in the LICENSE file.
// (C) Nandor Licker. All rights reserved.

#pragma once

#include <vector>



namespace miniml {
class Context;

/// Interpreter implementation.
class Interpreter {
 public:
  // Creates a new interpreter.
  Interpreter(Context &ctx, const uint32_t *code, Value global);

  // Frees the interpreter.
  ~Interpreter();

  // Interprets a bytecode file.
  void run();

 private:
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
  /// Reference to the context.
  Context &ctx;
  /// Code being executed.
  const uint32_t *code;
  /// Stack pointer.
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
};

} // namespace miniml
