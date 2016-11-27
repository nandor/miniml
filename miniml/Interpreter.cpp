// This file is part of the miniml project.
// Licensing information can be found in the LICENSE file.
// (C) Nandor Licker. All rights reserved.

#include "miniml/Context.h"
#include "miniml/Value.h"
#include "miniml/Interpreter.h"
using namespace miniml;



// -----------------------------------------------------------------------------
// Stack
// -----------------------------------------------------------------------------
void Stack::push(value value) {
  stack_.push_back(value);
}

value Stack::pop() {
  value val = *stack_.rbegin();
  stack_.pop_back();
  return val;
}

void Stack::pop_n(uint32_t n) {
  stack_.erase(stack_.end() - n, stack_.end());
}



// -----------------------------------------------------------------------------
// Interpreter
// -----------------------------------------------------------------------------
Interpreter::Interpreter(
    Context &ctx,
    const uint32_t *code,
    Value global,
    std::vector<void*> prim)
  : ctx(ctx)
  , code(code)
  , A(1ull)
  , global(global)
  , prim(prim)
{
  for (size_t i = 0; i < 256; ++i) {
    atom[i] = ctx.allocBlock(0, i);
  }
}

Interpreter::~Interpreter() {
}

void Interpreter::run() {
  PC = 0;
  for (;;) {
    auto op = code[PC++];
    switch (op) {
    case   0: runACC(0); break;
    case   1: runACC(1); break;
    case   2: runACC(2); break;
    case   3: runACC(3); break;
    case   4: runACC(4); break;
    case   5: runACC(5); break;
    case   6: runACC(6); break;
    case   7: runACC(7); break;
    case   8: runACC(code[PC++]); break;
    case   9: runPUSH(); break;
    case  10: runPUSH(); break;
    case  11: runPUSHACC(1); break;
    case  12: runPUSHACC(2); break;
    case  13: runPUSHACC(3); break;
    case  14: runPUSHACC(4); break;
    case  15: runPUSHACC(5); break;
    case  16: runPUSHACC(6); break;
    case  17: runPUSHACC(7); break;
    case  18: runPUSHACC(code[PC++]); break;
    case  19: runPOP(code[PC++]); break;

    case  32: runAPPLY(code[PC++]); break;
    case  33: runAPPLY(1); break;
    case  34: runAPPLY(2); break;
    case  35: runAPPLY(3); break;

    case  43: runCLOSURE(); break;
    case  44: runCLOSUREREC(); break;

    case  53: runGETGLOBAL(code[PC++]); break;
    case  54: runPUSHGETGLOBAL(code[PC++]); break;
    case  55: runGETGLOBALFIELD(); break;
    case  56: runPUSHGETGLOBALFIELD(); break;
    case  57: runSETGLOBAL(code[PC++]); break;
    case  58: runATOM(0); break;
    case  59: runATOM(code[PC++]); break;
    case  60: runPUSHATOM(0); break;
    case  61: runPUSHATOM(code[PC++]); break;

    case  62: runMAKEBLOCK(code[PC++]); break;
    case  63: runMAKEBLOCK(1); break;
    case  64: runMAKEBLOCK(2); break;
    case  65: runMAKEBLOCK(3); break;

    case  67: runGETFIELD(0); break;
    case  68: runGETFIELD(1); break;
    case  69: runGETFIELD(2); break;
    case  70: runGETFIELD(3); break;
    case  71: runGETFIELD(code[PC++]); break;

    case  84: runBRANCH(code[PC++]); break;
    case  85: runBRANCH(code[PC++]); break;
    case  86: runBRANCH(code[PC++]); break;

    case  93: runCCALL(1); break;
    case  94: runCCALL(2); break;
    case  95: runCCALL(3); break;
    case  96: runCCALL(4); break;
    case  97: runCCALL(5); break;
    case  98: runCCALL(code[PC++]); break;
    case  99: runCONST(0); break;
    case 100: runCONST(1); break;
    case 101: runCONST(2); break;
    case 102: runCONST(3); break;
    case 103: runCONST(code[PC++]); break;
    case 104: runPUSHCONST(0); break;
    case 105: runPUSHCONST(1); break;
    case 106: runPUSHCONST(2); break;
    case 107: runPUSHCONST(3); break;
    case 108: runPUSHCONST(code[PC++]); break;

    case 119: runLSRINT(); break;

    case 125: runGTINT(); break;
    case 127: runOFFSETINT(code[PC++]); break;

    default:
      throw std::runtime_error("Uknonwn opcode: " + std::to_string(op));
    }
    std::cout << "PC: " << PC << std::endl;
    std::cout << "Stack: " << stack.sp() << std::endl;
    std::cout << "Acc: "; printValue(ctx, A, std::cout);
    std::cout << "-----" << std::endl;
  }
}

// -----------------------------------------------------------------------------
void Interpreter::runACC(uint32_t n) {
  std::cout << "ACC" << std::endl;
  A = stack[n];
}

// -----------------------------------------------------------------------------
void Interpreter::runPUSH() {
  std::cout << "PUSH" << std::endl;
  stack.push(A);

}

// -----------------------------------------------------------------------------
void Interpreter::runPUSHACC(uint32_t n) {
  std::cout << "PUSHACC " << n << std::endl;
  stack.push(A);
  A = stack[n];
}

// -----------------------------------------------------------------------------
void Interpreter::runPOP(uint32_t n) {
  std::cout << "POP" << std::endl;
  stack.pop_n(n);
}

// -----------------------------------------------------------------------------
void Interpreter::runASSIGN() {
  std::cout << "ASSIGN" << std::endl;

}

// -----------------------------------------------------------------------------
void Interpreter::runENVACC() {
  std::cout << "ENVACC" << std::endl;

}

// -----------------------------------------------------------------------------
void Interpreter::runGETFIELD(uint32_t n) {
  std::cout << "GETFIELD" << std::endl;
  A = A.getField(n);
}

// -----------------------------------------------------------------------------
void Interpreter::runPUSH_RETADDR() {
  std::cout << "PUSH_RETADDR" << std::endl;

}

// -----------------------------------------------------------------------------
void Interpreter::runAPPLY(uint32_t args) {
  std::cout << "APPLY " << args << std::endl;
  printValue(ctx, A, std::cout);
  PC = A.getField(0).getInt64();
  std::cerr << "PC: " << PC << std::endl;
  extraArgs = args - 1;
  exit(0);
}

// -----------------------------------------------------------------------------
void Interpreter::runRESTART() {
  std::cout << "RESTART" << std::endl;

}

// -----------------------------------------------------------------------------
void Interpreter::runGRAB() {
  std::cout << "GRAB" << std::endl;

}

// -----------------------------------------------------------------------------
void Interpreter::runCLOSURE() {
  std::cout << "CLOSURE" << std::endl;
  uint32_t n = code[PC++];
  int32_t ofs = code[PC++];

  if (n > 0) {
    stack.push(A);
  }

  Value c = ctx.allocBlock(n + 1, kClosureTag);
  c.setField(0, PC + ofs - 1);
  for (uint32_t i = 0; i < n; ++i) {
    c.setField(i + 1, stack.pop());
  }
}

// -----------------------------------------------------------------------------
void Interpreter::runCLOSUREREC() {
  std::cout << "CLOSUREREC" << std::endl;
  uint32_t f = code[PC++];
  uint32_t v = code[PC++];

  if (v > 0) {
    stack.push(A);
  }

  A = ctx.allocBlock(2 * f - 1 + v, kClosureTag);
  stack.pop_n(v);
  for (uint32_t i = 0; i < f; ++i) {
    stack.push(Value(1ll));
  }
  PC += f;
  exit(0);
}

// -----------------------------------------------------------------------------
void Interpreter::runPUSHOFFSETCLOSURE() {
  std::cout << "PUSHOFFSETCLOSURE" << std::endl;

}

// -----------------------------------------------------------------------------
void Interpreter::runGETGLOBAL(uint32_t n) {
  std::cout << "GETGLOBAL" << std::endl;
  A = global.getField(n);
}

// -----------------------------------------------------------------------------
void Interpreter::runPUSHGETGLOBAL(uint32_t n) {
  std::cout << "PUSHGETGLOBAL" << std::endl;
  stack.push(A);
  A = global.getField(n);
}

// -----------------------------------------------------------------------------
void Interpreter::runGETGLOBALFIELD() {
  std::cout << "GETGLOBALFIELD" << std::endl;
  uint32_t n = code[PC++];
  uint32_t p = code[PC++];
  A = global.getField(n).getField(p);

}

// -----------------------------------------------------------------------------
void Interpreter::runPUSHGETGLOBALFIELD() {
  std::cout << "PUSHGETGLOBALFIELD" << std::endl;
  uint32_t n = code[PC++];
  uint32_t p = code[PC++];
  stack.push(A);
  A = global.getField(n).getField(p);
}

// -----------------------------------------------------------------------------
void Interpreter::runSETGLOBAL(uint32_t n) {
  std::cout << "SETGLOBAL" << std::endl;
  global.setField(n, A);
  A = kUnit;
}

// -----------------------------------------------------------------------------
void Interpreter::runATOM(uint32_t n) {
  std::cout << "ATOM " << n << std::endl;
  A = atom[n];

}

// -----------------------------------------------------------------------------
void Interpreter::runPUSHATOM(uint32_t n) {
  std::cout << "PUSHATOM " << n << std::endl;
  stack.push(A);
  A = atom[n];
}

// -----------------------------------------------------------------------------
void Interpreter::runMAKEBLOCK(uint32_t n) {
  std::cout << "MAKEBLOCK" << std::endl;
  uint32_t t = code[PC++];
  Value block = ctx.allocBlock(n, t);
  block.setField(0, A);
  for (uint32_t i = 1; i < n; ++i) {
    block.setField(i, stack.pop());
  }
  A = block;
}

// -----------------------------------------------------------------------------
void Interpreter::runBRANCH(int32_t ofs) {
  std::cout << "BRANCH" << std::endl;
  PC += ofs - 1;
}

// -----------------------------------------------------------------------------
void Interpreter::runBRANCHIF(int32_t ofs) {
  std::cout << "BRANCHIF" << std::endl;

}

// -----------------------------------------------------------------------------
void Interpreter::runBRANCHIFNOT(int32_t ofs) {
  std::cout << "BRANCHIFNOT" << std::endl;

}

// -----------------------------------------------------------------------------
void Interpreter::runSWITCH() {
  std::cout << "SWITCH" << std::endl;

}

// -----------------------------------------------------------------------------
void Interpreter::runPUSHTRAP() {
  std::cout << "PUSHTRAP" << std::endl;

}

// -----------------------------------------------------------------------------
void Interpreter::runPOPTRAP() {
  std::cout << "POPTRAP" << std::endl;

}

// -----------------------------------------------------------------------------
void Interpreter::runCHECK_SIGNALS() {
  std::cout << "CHECK_SIGNALS" << std::endl;
}

// -----------------------------------------------------------------------------
void Interpreter::runCONST(int32_t n) {
  std::cout << "CONST" << std::endl;
  A = Value(static_cast<int64_t>(n) << 1ll | 1ll);
}

// -----------------------------------------------------------------------------
void Interpreter::runPUSHCONST(int32_t n) {
  std::cout << "PUSHCONST" << std::endl;
  stack.push(A);
  A = Value(static_cast<int64_t>(n) << 1ll | 1ll);
}

// -----------------------------------------------------------------------------
void Interpreter::runCCALL(uint32_t n) {
  std::cout << "CCALL" << n << std::endl;
  uint32_t p = code[PC++];

  stack.push(env);

  switch (n) {
  case 0: {
    auto *fn = ((value(*)(Context&, value))prim[p]);
    A = fn(ctx, A);
    break;
  }
  case 1: {
    auto *fn = ((value(*)(Context&, value, value))prim[p]);
    A = fn(ctx, A, stack[0]);
    break;
  }
  case 2: {
    auto *fn = ((value(*)(Context&, value, value, value))prim[p]);
    A = fn(ctx, A, stack[0], stack[1]);
    break;
  }
  case 3: {
    auto *fn = ((value(*)(Context&, value, value, value, value))prim[p]);
    A = fn(ctx, A, stack[0], stack[1], stack[2]);
    break;
  }
  case 4: {
    auto *fn = ((value(*)(Context&, value, value, value, value, value))prim[p]);
    A = fn(ctx, A, stack[0], stack[1], stack[2], stack[3]);
    break;
  }
  default:
    throw std::runtime_error("CCall not implemented");
  }

  env = stack.pop();
  stack.pop_n(n - 1);
}

// -----------------------------------------------------------------------------
void Interpreter::runLSRINT() {
  std::cout << "LSRINT" << std::endl;
  int64_t i = val_to_int64(stack.pop());
  A = ctx.allocInt64(static_cast<uint64_t>(A.getInt64()) >> i);
}

// -----------------------------------------------------------------------------
void Interpreter::runOFFSETINT(int32_t ofs) {
  std::cout << "OFFSETINT " << ofs << std::endl;
  A = ctx.allocInt64(A.getInt64() + ofs);
}

// -----------------------------------------------------------------------------
void Interpreter::runGTINT() {
  std::cout << "GTINT" << std::endl;
  if (A.getInt64() > stack.pop()) {
    A = kTrue;
  } else {
    A = kFalse;
  }
}
