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
void Stack::push(Value value) {
  stack_.push_back(value);
}

Value Stack::pop() {
  Value val = *stack_.rbegin();
  stack_.pop_back();
  return val;
}

void Stack::pop_n(uint32_t n) {
  stack_.erase(stack_.end() - n, stack_.end());
}

Value &Stack::operator[](unsigned n) {
  return *(stack_.rbegin() + n);
}

unsigned Stack::getSP() const {
  return stack_.size();
}

void Stack::setSP(unsigned sp) {
  stack_.erase(stack_.begin() + sp, stack_.end());
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
  , extraArgs(0)
  , global(global)
  , prim(prim)
{
}

Interpreter::~Interpreter() {
}

Value Interpreter::run() {
  PC = 0;
  long l = 0;
  for (;;) {
    switch (auto op = code[PC++]) {
    case   0: runACC(0);                        break;
    case   1: runACC(1);                        break;
    case   2: runACC(2);                        break;
    case   3: runACC(3);                        break;
    case   4: runACC(4);                        break;
    case   5: runACC(5);                        break;
    case   6: runACC(6);                        break;
    case   7: runACC(7);                        break;
    case   8: runACC(code[PC++]);               break;
    case   9: runPUSH();                        break;
    case  10: runPUSH();                        break;
    case  11: runPUSHACC(1);                    break;
    case  12: runPUSHACC(2);                    break;
    case  13: runPUSHACC(3);                    break;
    case  14: runPUSHACC(4);                    break;
    case  15: runPUSHACC(5);                    break;
    case  16: runPUSHACC(6);                    break;
    case  17: runPUSHACC(7);                    break;
    case  18: runPUSHACC(code[PC++]);           break;
    case  19: runPOP(code[PC++]);               break;
    case  20: runASSIGN(code[PC++]);            break;
    case  21: runENVACC(1);                     break;
    case  22: runENVACC(2);                     break;
    case  23: runENVACC(3);                     break;
    case  24: runENVACC(4);                     break;
    case  25: runENVACC(code[PC++]);            break;
    case  26: runPUSHENVACC(1);                 break;
    case  27: runPUSHENVACC(2);                 break;
    case  28: runPUSHENVACC(3);                 break;
    case  29: runPUSHENVACC(4);                 break;
    case  30: runPUSHENVACC(code[PC++]);        break;
    case  31: runPUSH_RETADDR(code[PC++]);      break;
    case  32: runAPPLY(code[PC++]);             break;
    case  33: runAPPLY1();                      break;
    case  34: runAPPLY2();                      break;
    case  35: runAPPLY3();                      break;
    case  36: runAPPTERM();                     break;
    case  37: runAPPTERM1();                    break;
    case  38: runAPPTERM2();                    break;
    case  39: runAPPTERM3();                    break;
    case  40: runRETURN(code[PC++]);            break;
    case  41: runRESTART();                     break;
    case  42: runGRAB(code[PC++]);              break;
    case  43: runCLOSURE();                     break;
    case  44: runCLOSUREREC();                  break;
    case  45: runOFFSETCLOSUREM2();             break;
    case  46: runOFFSETCLOSURE(0);              break;
    case  47: runOFFSETCLOSURE(2);              break;
    case  48: runOFFSETCLOSURE(code[PC++]);     break;
    case  49: runPUSHOFFSETCLOSUREM2();         break;
    case  50: runPUSHOFFSETCLOSURE(0);          break;
    case  51: runPUSHOFFSETCLOSURE(2);          break;
    case  52: runPUSHOFFSETCLOSURE(code[PC++]); break;
    case  53: runGETGLOBAL(code[PC++]);         break;
    case  54: runPUSHGETGLOBAL(code[PC++]);     break;
    case  55: runGETGLOBALFIELD();              break;
    case  56: runPUSHGETGLOBALFIELD();          break;
    case  57: runSETGLOBAL(code[PC++]);         break;
    case  58: runATOM(0);                       break;
    case  59: runATOM(code[PC++]);              break;
    case  60: runPUSHATOM(0);                   break;
    case  61: runPUSHATOM(code[PC++]);          break;
    case  62: runMAKEBLOCK(code[PC++]);         break;
    case  63: runMAKEBLOCK(1);                  break;
    case  64: runMAKEBLOCK(2);                  break;
    case  65: runMAKEBLOCK(3);                  break;
    case  66: runMAKEFLOATBLOCK(code[PC++]);    break;
    case  67: runGETFIELD(0);                   break;
    case  68: runGETFIELD(1);                   break;
    case  69: runGETFIELD(2);                   break;
    case  70: runGETFIELD(3);                   break;
    case  71: runGETFIELD(code[PC++]);          break;
    case  72: runGETFLOATFIELD(code[PC++]);     break;
    case  73: runSETFIELD(0);                   break;
    case  74: runSETFIELD(1);                   break;
    case  75: runSETFIELD(2);                   break;
    case  76: runSETFIELD(3);                   break;
    case  77: runSETFIELD(code[PC++]);          break;
    case  78: runSETFLOATFIELD(code[PC++]);     break;
    case  79: runVECTLENGTH();                  break;
    case  80: runGETVECTITEM();                 break;
    case  81: runSETVECTITEM();                 break;
    case  82: runGETSTRINGCHAR();               break;
    case  83: runSETSTRINGCHAR();               break;
    case  84: runBRANCH(code[PC++]);            break;
    case  85: runBRANCHIF(code[PC++]);          break;
    case  86: runBRANCHIFNOT(code[PC++]);       break;
    case  87: runSWITCH();                      break;
    case  88: runBOOLNOT();                     break;
    case  89: runPUSHTRAP(code[PC++]);          break;
    case  90: runPOPTRAP();                     break;
    case  91: runRAISE();                       break;
    case  92: runCHECK_SIGNALS();               break;
    case  93: runCCALL(1);                      break;
    case  94: runCCALL(2);                      break;
    case  95: runCCALL(3);                      break;
    case  96: runCCALL(4);                      break;
    case  97: runCCALL(5);                      break;
    case  98: runCCALL(code[PC++]);             break;
    case  99: runCONST(0);                      break;
    case 100: runCONST(1);                      break;
    case 101: runCONST(2);                      break;
    case 102: runCONST(3);                      break;
    case 103: runCONST(code[PC++]);             break;
    case 104: runPUSHCONST(0);                  break;
    case 105: runPUSHCONST(1);                  break;
    case 106: runPUSHCONST(2);                  break;
    case 107: runPUSHCONST(3);                  break;
    case 108: runPUSHCONST(code[PC++]);         break;
    case 109: runNEGINT();                      break;
    case 110: runADDINT();                      break;
    case 111: runSUBINT();                      break;
    case 112: runMULINT();                      break;
    case 113: runDIVINT();                      break;
    case 114: runMODINT();                      break;
    case 115: runANDINT();                      break;
    case 116: runORINT();                       break;
    case 117: runXORINT();                      break;
    case 118: runLSLINT();                      break;
    case 119: runLSRINT();                      break;
    case 120: runASRINT();                      break;
    case 121: runEQ();                          break;
    case 122: runNEQ();                         break;
    case 123: runLTINT();                       break;
    case 124: runLEINT();                       break;
    case 125: runGTINT();                       break;
    case 126: runGEINT();                       break;
    case 127: runOFFSETINT(code[PC++]);         break;
    case 128: runOFFSETREF();                   break;
    case 129: runISINT();                       break;
    case 130: runGETMETHOD();                   break;
    case 131: runBEQ();                         break;
    case 132: runBNEQ();                        break;
    case 133: runBLTINT();                      break;
    case 134: runBLEINT();                      break;
    case 135: runBGTINT();                      break;
    case 136: runBGEINT();                      break;
    case 137: runULTINT();                      break;
    case 138: runUGEINT();                      break;
    case 139: runBULTINT();                     break;
    case 140: runBUGEINT();                     break;
    case 141: runGETPUBMET();                   break;
    case 142: runGETDYNMET();                   break;
    case 143: return A;
    case 144: runEVENT();                       break;
    case 145: runBREAK();                       break;
    default:
      throw std::runtime_error("Uknonwn opcode: " + std::to_string(op));
    }
    std::cout << ++l << " " << stack.getSP() << " " << extraArgs << " " << (A & 1 ? (int64_t)A : 0) /*<< " " << code[PC]*/ << std::endl;
  }
}

// -----------------------------------------------------------------------------
void Interpreter::runACC(uint32_t n) {
  A = stack[n];
}

// -----------------------------------------------------------------------------
void Interpreter::runPUSH() {
  stack.push(A);

}

// -----------------------------------------------------------------------------
void Interpreter::runPUSHACC(uint32_t n) {
  stack.push(A);
  A = stack[n];
}

// -----------------------------------------------------------------------------
void Interpreter::runPOP(uint32_t n) {
  stack.pop_n(n);
}

// -----------------------------------------------------------------------------
void Interpreter::runASSIGN(uint32_t n) {
  stack[n] = A;
  A = kUnit;
}

// -----------------------------------------------------------------------------
void Interpreter::runGETFIELD(uint32_t n) {
  A = A.getField(n);
}

// -----------------------------------------------------------------------------
void Interpreter::runGETFLOATFIELD(uint32_t n) {
  std::cerr << "GETFLOATFIELD" << std::endl;
  (void) n;
  throw std::runtime_error("GETFLOATFIELD");
}

// -----------------------------------------------------------------------------
void Interpreter::runSETFIELD(uint32_t n) {
  A.setField(n, stack.pop());
  A = kUnit;
}

// -----------------------------------------------------------------------------
void Interpreter::runSETFLOATFIELD(uint32_t n) {
  (void) n;
  throw std::runtime_error("SETFLOATFIELD");
}

// -----------------------------------------------------------------------------
void Interpreter::runVECTLENGTH() {
  A = val_int64(val_size(A));
}

// -----------------------------------------------------------------------------
void Interpreter::runGETVECTITEM() {
  throw std::runtime_error("GETVECTITEM");
}

// -----------------------------------------------------------------------------
void Interpreter::runSETVECTITEM() {
  throw std::runtime_error("SETVECTITEM");
}

// -----------------------------------------------------------------------------
void Interpreter::runGETSTRINGCHAR() {
  throw std::runtime_error("GETSTRINGCHAR");
}

// -----------------------------------------------------------------------------
void Interpreter::runSETSTRINGCHAR() {
  throw std::runtime_error("SETSTRINGCHAR");
}


// -----------------------------------------------------------------------------
void Interpreter::runENVACC(uint32_t n) {
  A = val_field(env, n);
}

// -----------------------------------------------------------------------------
void Interpreter::runPUSHENVACC(uint32_t n) {
  stack.push(A);
  A = val_field(env, n);
}

// -----------------------------------------------------------------------------
void Interpreter::runPUSH_RETADDR(int32_t ofs) {
  stack.push(val_int64(extraArgs));
  stack.push(env);
  stack.push(val_int64(PC + ofs - 1));
}

// -----------------------------------------------------------------------------
void Interpreter::runAPPLY(uint32_t args) {
  PC = A.getCode();
  env = A;
  extraArgs = args - 1;
}

// -----------------------------------------------------------------------------
void Interpreter::runAPPLY1() {
  value arg = stack.pop();
  stack.push(val_int64(extraArgs));
  stack.push(env);
  stack.push(val_int64(PC));
  stack.push(arg);
  PC = A.getCode();
  env = A;
  extraArgs = 0;
}

// -----------------------------------------------------------------------------
void Interpreter::runAPPLY2() {
  value arg2 = stack.pop();
  value arg1 = stack.pop();
  stack.push(val_int64(extraArgs));
  stack.push(env);
  stack.push(val_int64(PC));
  stack.push(arg1);
  stack.push(arg2);
  PC = A.getCode();
  env = A;
  extraArgs = 1;
}

// -----------------------------------------------------------------------------
void Interpreter::runAPPLY3() {
  value arg3 = stack.pop();
  value arg2 = stack.pop();
  value arg1 = stack.pop();
  stack.push(val_int64(extraArgs));
  stack.push(env);
  stack.push(val_int64(PC));
  stack.push(arg1);
  stack.push(arg2);
  stack.push(arg3);
  PC = A.getCode();
  env = A;
  extraArgs = 2;
}

// -----------------------------------------------------------------------------
void Interpreter::runAPPTERM() {
  uint32_t n = code[PC++];
  uint32_t s = code[PC++];

  for (int32_t i = n - 1; i >= 0; --i) {
    stack[s - n + i] = stack[i];
  }
  stack.pop_n(s - n);

  PC = A.getCode();
  env = A;
  extraArgs += n - 1;
}

// -----------------------------------------------------------------------------
void Interpreter::runAPPTERM1() {
  uint32_t n = code[PC++];
  value arg1 = stack.pop();
  stack.pop_n(n - 1);
  stack.push(arg1);
  PC = A.getCode();
  env = A;
}

// -----------------------------------------------------------------------------
void Interpreter::runAPPTERM2() {
  uint32_t n = code[PC++];
  value arg2 = stack.pop();
  value arg1 = stack.pop();
  stack.pop_n(n - 2);
  stack.push(arg1);
  stack.push(arg2);
  PC = A.getCode();
  env = A;
  extraArgs += 1;
}

// -----------------------------------------------------------------------------
void Interpreter::runAPPTERM3() {
  uint32_t n = code[PC++];
  value arg3 = stack.pop();
  value arg2 = stack.pop();
  value arg1 = stack.pop();
  stack.pop_n(n - 3);
  stack.push(arg1);
  stack.push(arg2);
  stack.push(arg3);
  PC = A.getCode();
  env = A;
  extraArgs += 2;
}

// -----------------------------------------------------------------------------
void Interpreter::runRESTART() {
  long n = val_size(env) - 2;
  for (long i = n - 1; i >= 0; --i) {
    stack.push(val_field(env, i + 2));
  }
  env = val_field(env, 1);
  extraArgs += n;
}

// -----------------------------------------------------------------------------
void Interpreter::runGRAB(uint32_t n) {
  if (extraArgs >= n) {
    extraArgs -= n;
  } else {
    A = ctx.allocBlock(extraArgs + 3, kClosureTag);
    A.setCode(PC - 3);
    A.setField(1, env);
    for (size_t i = 0; i < extraArgs + 1; ++i) {
      A.setField(2 + i, stack.pop());
    }
    PC = val_to_int64(stack.pop());
    env = stack.pop();
    extraArgs = val_to_int64(stack.pop());
  }
}

// -----------------------------------------------------------------------------
void Interpreter::runCLOSURE() {
  uint32_t n = code[PC++];
  int32_t ofs = code[PC++];

  if (n > 0) {
    stack.push(A);
  }

  A = ctx.allocBlock(n + 1, kClosureTag);
  A.setCode(PC + ofs - 1);
  for (uint32_t i = 0; i < n; ++i) {
    A.setField(i + 1, stack.pop());
  }
}

// -----------------------------------------------------------------------------
void Interpreter::runCLOSUREREC() {
  uint32_t f = code[PC++];
  uint32_t v = code[PC++];

  if (v > 0) {
    stack.push(A);
  }

  A = ctx.allocBlock(f * 2 - 1 + v, kClosureTag);
  for (uint32_t i = 0; i < v; ++i) {
    A.setField(f * 2 - 1 + i, stack[i]);
  }
  stack.pop_n(v);
  A.setCode(PC + static_cast<int32_t>(code[PC]));
  stack.push(A);
  for (uint32_t i = 0; i + 1< f; ++i) {
    Value header((i * 2) << 10 | kInfixTag);
    A.setField(1 + i * 2, header);
    Value ofs(PC + code[PC + i]);
    A.setField(2 + i * 2, ofs);
    stack.push(ofs);
  }

  PC += f;
}

// -----------------------------------------------------------------------------
void Interpreter::runOFFSETCLOSUREM2() {
  throw std::runtime_error("OFFSETCLOSUREM2");
}

// -----------------------------------------------------------------------------
void Interpreter::runOFFSETCLOSURE(uint32_t n) {
  (void) n;
  throw std::runtime_error("OFFSETCLOSURE");
}

// -----------------------------------------------------------------------------
void Interpreter::runPUSHOFFSETCLOSUREM2() {
  throw std::runtime_error("PUSHOFFSETCLOSUREM2");
}

// -----------------------------------------------------------------------------
void Interpreter::runPUSHOFFSETCLOSURE(uint32_t n) {
  stack.push(A);
  A = env - n * sizeof(value);
}

// -----------------------------------------------------------------------------
void Interpreter::runGETGLOBAL(uint32_t n) {
  A = global.getField(n);
}

// -----------------------------------------------------------------------------
void Interpreter::runPUSHGETGLOBAL(uint32_t n) {
  stack.push(A);
  A = global.getField(n);
}

// -----------------------------------------------------------------------------
void Interpreter::runGETGLOBALFIELD() {
  uint32_t n = code[PC++];
  uint32_t p = code[PC++];
  A = global.getField(n).getField(p);

}

// -----------------------------------------------------------------------------
void Interpreter::runPUSHGETGLOBALFIELD() {
  uint32_t n = code[PC++];
  uint32_t p = code[PC++];
  stack.push(A);
  A = global.getField(n).getField(p);
}

// -----------------------------------------------------------------------------
void Interpreter::runSETGLOBAL(uint32_t n) {
  global.setField(n, A);
  A = kUnit;
}

// -----------------------------------------------------------------------------
void Interpreter::runATOM(uint32_t n) {
  A = ctx.allocAtom(n);
}

// -----------------------------------------------------------------------------
void Interpreter::runPUSHATOM(uint32_t n) {
  stack.push(A);
  A = ctx.allocAtom(n);
}

// -----------------------------------------------------------------------------
void Interpreter::runMAKEBLOCK(uint32_t n) {
  uint32_t t = code[PC++];
  Value block = ctx.allocBlock(n, t);
  block.setField(0, A);
  for (uint32_t i = 1; i < n; ++i) {
    block.setField(i, stack.pop());
  }
  A = block;
}

// -----------------------------------------------------------------------------
void Interpreter::runMAKEFLOATBLOCK(uint32_t n) {
  (void) n;
  throw std::runtime_error("MAKEFLOATBLOCK");
}

// -----------------------------------------------------------------------------
void Interpreter::runBRANCH(int32_t ofs) {
  PC += ofs - 1;
}

// -----------------------------------------------------------------------------
void Interpreter::runBRANCHIF(int32_t ofs) {
  if (A != kFalse) {
    PC += ofs - 1;
  }
}

// -----------------------------------------------------------------------------
void Interpreter::runBRANCHIFNOT(int32_t ofs) {
  if (A == kFalse) {
    PC += ofs - 1;
  }
}

// -----------------------------------------------------------------------------
void Interpreter::runSWITCH() {
  std::cerr << "SWITCH" << std::endl;
  throw std::runtime_error("SWITCH");
}

// -----------------------------------------------------------------------------
void Interpreter::runBOOLNOT() {
  std::cerr << "BOOLNOT" << std::endl;
  throw std::runtime_error("BOOLNOT");
}

// -----------------------------------------------------------------------------
void Interpreter::runPUSHTRAP(int32_t ofs) {
  stack.push(val_int64(extraArgs));
  stack.push(env);
  stack.push(val_int64(trapSP));
  stack.push(val_int64(PC + ofs - 1));
  trapSP = stack.getSP();
}

// -----------------------------------------------------------------------------
void Interpreter::runPOPTRAP() {
  trapSP = val_to_int64(stack[1]);
  stack.pop_n(4);
}

// -----------------------------------------------------------------------------
void Interpreter::runCHECK_SIGNALS() {
  //std::cerr << "CHECK_SIGNALS" << std::endl;
}

// -----------------------------------------------------------------------------
void Interpreter::runCONST(int32_t n) {
  A = ctx.allocInt64(n);
}

// -----------------------------------------------------------------------------
void Interpreter::runPUSHCONST(int32_t n) {
  stack.push(A);
  A = ctx.allocInt64(n);
}

// -----------------------------------------------------------------------------
void Interpreter::runCCALL(uint32_t n) {
  uint32_t p = code[PC++];
  auto *ptr = prim[p];
  if (ptr == nullptr) {
    throw std::runtime_error("Undefined prim " + std::to_string(p));
  }

  stack.push(env);

  switch (n) {
  case 1: {
    auto *fn = ((value(*)(Context&, value))ptr);
    A = fn(ctx, A);
    break;
  }
  case 2: {
    auto *fn = ((value(*)(Context&, value, value))ptr);
    A = fn(ctx, A, stack[1]);
    break;
  }
  case 3: {
    auto *fn = ((value(*)(Context&, value, value, value))ptr);
    A = fn(ctx, A, stack[1], stack[2]);
    break;
  }
  case 4: {
    auto *fn = ((value(*)(Context&, value, value, value, value))ptr);
    A = fn(ctx, A, stack[1], stack[2], stack[3]);
    break;
  }
  case 5: {
    auto *fn = ((value(*)(Context&, value, value, value, value, value))ptr);
    A = fn(ctx, A, stack[1], stack[2], stack[3], stack[4]);
    break;
  }
  default:
    throw std::runtime_error("CCall not implemented");
  }

  env = stack.pop();
  stack.pop_n(n - 1);
}

// -----------------------------------------------------------------------------
void Interpreter::runMULINT() {
  int64_t i = val_to_int64(stack.pop());
  A = ctx.allocInt64(static_cast<uint64_t>(A.getInt64()) * i);
}

// -----------------------------------------------------------------------------
void Interpreter::runNEGINT() {
}

// -----------------------------------------------------------------------------
void Interpreter::runADDINT() {
  int64_t i = val_to_int64(stack.pop());
  A = ctx.allocInt64(static_cast<uint64_t>(A.getInt64()) + i);
}

// -----------------------------------------------------------------------------
void Interpreter::runSUBINT() {
  int64_t i = val_to_int64(stack.pop());
  A = ctx.allocInt64(static_cast<uint64_t>(A.getInt64()) - i);
}

// -----------------------------------------------------------------------------
void Interpreter::runDIVINT() {
  int64_t i = val_to_int64(stack.pop());
  if (i == 0) {
    throw std::runtime_error("Division by zero.");
  } else {
    A = ctx.allocInt64(static_cast<uint64_t>(A.getInt64()) / i);
  }
}

// -----------------------------------------------------------------------------
void Interpreter::runMODINT() {
  int64_t i = val_to_int64(stack.pop());
  if (i == 0) {
    throw std::runtime_error("Division by zero.");
  } else {
    A = ctx.allocInt64(static_cast<uint64_t>(A.getInt64()) % i);
  }
}

// -----------------------------------------------------------------------------
void Interpreter::runANDINT() {
  A = static_cast<value>(A) & static_cast<value>(stack.pop());
}

// -----------------------------------------------------------------------------
void Interpreter::runORINT() {
  A = static_cast<value>(A) | static_cast<value>(stack.pop());
}

// -----------------------------------------------------------------------------
void Interpreter::runXORINT() {
  A = (static_cast<value>(A) ^ static_cast<value>(stack.pop())) | 1;
}

// -----------------------------------------------------------------------------
void Interpreter::runLSLINT() {
  A = ((static_cast<value>(A) - 1) << stack.pop().getInt64()) + 1;
}

// -----------------------------------------------------------------------------
void Interpreter::runLSRINT() {
  A = ((static_cast<uint64_t>(A) - 1) >> stack.pop().getInt64()) | 1;
}

// -----------------------------------------------------------------------------
void Interpreter::runASRINT() {
  A = ((static_cast<value>(A) - 1) >> stack.pop().getInt64()) | 1;
}

// -----------------------------------------------------------------------------
void Interpreter::runOFFSETINT(int32_t ofs) {
  A = ctx.allocInt64(A.getInt64() + ofs);
}

// -----------------------------------------------------------------------------
void Interpreter::runOFFSETREF() {
  val_field(A, 0) += val_to_int64(code[PC++]);
  A = kUnit;
}

// -----------------------------------------------------------------------------
void Interpreter::runISINT() {
  throw std::runtime_error("ISINT");
}

// -----------------------------------------------------------------------------
void Interpreter::runGETMETHOD() {
  throw std::runtime_error("GETMETHOD");
}

// -----------------------------------------------------------------------------
void Interpreter::runEQ() {
  if (A == stack.pop()) {
    A = kTrue;
  } else {
    A = kFalse;
  }
}

// -----------------------------------------------------------------------------
void Interpreter::runNEQ() {
  if (A != stack.pop()) {
    A = kTrue;
  } else {
    A = kFalse;
  }
}

// -----------------------------------------------------------------------------
void Interpreter::runLTINT() {
  if (A.getInt64() < val_to_int64(stack.pop())) {
    A = kTrue;
  } else {
    A = kFalse;
  }
}

// -----------------------------------------------------------------------------
void Interpreter::runLEINT() {
  if (A.getInt64() <= val_to_int64(stack.pop())) {
    A = kTrue;
  } else {
    A = kFalse;
  }
}

// -----------------------------------------------------------------------------
void Interpreter::runGTINT() {
  if (A.getInt64() > val_to_int64(stack.pop())) {
    A = kTrue;
  } else {
    A = kFalse;
  }
}

// -----------------------------------------------------------------------------
void Interpreter::runGEINT() {
  if (A.getInt64() >= val_to_int64(stack.pop())) {
    A = kTrue;
  } else {
    A = kFalse;
  }
}

// -----------------------------------------------------------------------------
void Interpreter::runBEQ() {
  auto v = static_cast<uint32_t>(code[PC++]);
  auto ofs = static_cast<int32_t>(code[PC++]);
  if (v == A.getInt64()) {
    PC += ofs - 1;
  }
}

// -----------------------------------------------------------------------------
void Interpreter::runBNEQ() {
  auto v = static_cast<uint32_t>(code[PC++]);
  auto ofs = static_cast<int32_t>(code[PC++]);
  if (v != A.getInt64()) {
    PC += ofs - 1;
  }
}

// -----------------------------------------------------------------------------
void Interpreter::runRETURN(uint32_t n) {
  stack.pop_n(n);
  if (extraArgs > 0) {
    extraArgs -= 1;
    PC = A.getCode();
    env = A;
  } else {
    PC = val_to_int64(stack.pop());
    env = stack.pop();
    extraArgs = val_to_int64(stack.pop());
  }
}

// -----------------------------------------------------------------------------
void Interpreter::runRAISE() {
  if (trapSP == 0) {
    std::runtime_error("No exception handler.");
  }
  stack.setSP(trapSP);
  PC = val_to_int64(stack.pop());
  trapSP = val_to_int64(stack.pop());
  env = stack.pop();
  extraArgs = val_to_int64(stack.pop());
}

// -----------------------------------------------------------------------------
void Interpreter::runBLTINT() {
  if (static_cast<uint32_t>(code[PC++]) < A.getInt64()) {
    PC += static_cast<int32_t>(code[PC]);
  } else {
    PC += 1;
  }
}

// -----------------------------------------------------------------------------
void Interpreter::runBLEINT() {
  if (static_cast<uint32_t>(code[PC++]) <= A.getInt64()) {
    PC += static_cast<int32_t>(code[PC]);
  } else {
    PC += 1;
  }
}

// -----------------------------------------------------------------------------
void Interpreter::runBGTINT() {
  if (static_cast<uint32_t>(code[PC++]) > A.getInt64()) {
    PC += static_cast<int32_t>(code[PC]);
  } else {
    PC += 1;
  }
}

// -----------------------------------------------------------------------------
void Interpreter::runBGEINT() {
  if (static_cast<uint32_t>(code[PC++]) >= A.getInt64()) {
    PC += static_cast<int32_t>(code[PC]);
  } else {
    PC += 1;
  }
}

// -----------------------------------------------------------------------------
void Interpreter::runULTINT() {
  throw std::runtime_error("ULTINT");
}

// -----------------------------------------------------------------------------
void Interpreter::runUGEINT() {
  throw std::runtime_error("UGEINT");
}

// -----------------------------------------------------------------------------
void Interpreter::runBULTINT() {
  throw std::runtime_error("BULTINT");
}

// -----------------------------------------------------------------------------
void Interpreter::runBUGEINT() {
  throw std::runtime_error("BUGEINT");
}

// -----------------------------------------------------------------------------
void Interpreter::runGETPUBMET() {
  throw std::runtime_error("GETPUBMET");
}

// -----------------------------------------------------------------------------
void Interpreter::runGETDYNMET() {
  throw std::runtime_error("GETDYNMET");
}

// -----------------------------------------------------------------------------
void Interpreter::runEVENT() {
  throw std::runtime_error("EVENT");
}

// -----------------------------------------------------------------------------
void Interpreter::runBREAK() {
  throw std::runtime_error("BREAK");
}
