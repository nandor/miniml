// This file is part of the miniml project.
// Licensing information can be found in the LICENSE file.
// (C) Nandor Licker. All rights reserved.

#pragma once

#include <vector>



namespace miniml {
class Context;

/// Interpreter stack.
class Stack {
 public:
  /// Pushes a value onto the stack.
  void push(value value);
  /// Pops a value from the stack.
  value pop();
  /// Pops n values from the stack.
  void pop_n(unsigned n);
  /// Peeks at a stack value.
  value operator[](unsigned n) {
    return stack_[n];
  }
  /// Returns the stack pointer value.
  unsigned sp() const {
    return stack_.size();
  }

 private:
  /// Vector containing stack values.
  std::vector<value> stack_;
};



/// Interpreter implementation.
class Interpreter {
 public:
  // Creates a new interpreter.
  Interpreter(
      Context &ctx,
      const uint32_t *code,
      Value global,
      std::vector<void*> prim);

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
  void runGETFIELD(uint32_t n);
  void runENVACC(uint32_t n);
  void runPUSHENVACC(uint32_t n);
  void runPUSH_RETADDR();
  void runAPPLY(uint32_t args);
  void runAPPLY1();
  void runAPPLY2();
  void runAPPLY3();
  void runAPPTERM2();
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
  void runATOM(uint32_t n);
  void runPUSHATOM(uint32_t n);
  void runMAKEBLOCK(uint32_t n);
  void runBRANCH(int32_t ofs);
  void runBRANCHIF(int32_t ofs);
  void runBRANCHIFNOT(int32_t ofs);
  void runSWITCH();
  void runPUSHTRAP();
  void runPOPTRAP();
  void runCHECK_SIGNALS();
  void runCONST(int32_t n);
  void runPUSHCONST(int32_t n);
  void runCCALL(uint32_t n);
  void runLSRINT();
  void runOFFSETINT(int32_t ofs);
  void runGTINT();

 private:
  /// Reference to the context.
  Context &ctx;
  /// Builtin functions.
  std::vector<void *> prim;
  /// Code being executed.
  const uint32_t *code;
  /// Stack.
  Stack stack;
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
  /// Atom table.
  Value atom[256];
};

} // namespace miniml
