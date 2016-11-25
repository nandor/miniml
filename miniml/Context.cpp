// This file is part of the miniml project.
// Licensing information can be found in the LICENSE file.
// (C) Nandor Licker. All rights reserved.

#include "miniml/BytecodeFile.h"
#include "miniml/Context.h"
#include "miniml/Stream.h"
#include "miniml/Value.h"
using namespace miniml;



// -----------------------------------------------------------------------------
// Context
// -----------------------------------------------------------------------------
Value Context::allocInt64(int64_t i) {
  return Value(i);
}

Value Context::allocDouble(double v) {
  return Value(v);
}

Value Context::allocString(const char *str, size_t length) {
  return Value(str, length);
}

Value Context::allocBlock(size_t n, uint8_t tag) {
  return Value(n, tag);
}

void Context::run(BytecodeFile &file) {
  // Decode the global data.
  auto dataSection = file.getSection(DATA);
  MemoryStreamReader codeStream(dataSection->getData(), dataSection->getSize());

  // Decode the code.
  auto codeSection = file.getSection(CODE);
  code = reinterpret_cast<const uint32_t *>(codeSection->getData());
  global = getValue(*this, codeStream);

  // Decode PRIM.
  auto primSection = file.getSection(PRIM);
  MemoryStreamReader primStream(primSection->getData(), primSection->getSize());
  while (!primStream.eof()) {
    prim_.push_back(primStream.getString());
  }

  PC = 0;
  for (;;) {
    auto op = code[PC++];
    std::cerr << PC << " " << op << std::endl;
    switch (op) {
    case  0: runACC(0); break;
    case  1: runACC(1); break;
    case  2: runACC(2); break;
    case  3: runACC(3); break;
    case  4: runACC(4); break;
    case  5: runACC(5); break;
    case  6: runACC(6); break;
    case  7: runACC(7); break;
    case  8: runACC(code[PC++]); break;

    case 10: runPUSHACC(0); break;
    case 11: runPUSHACC(1); break;
    case 12: runPUSHACC(2); break;
    case 13: runPUSHACC(3); break;
    case 14: runPUSHACC(4); break;
    case 15: runPUSHACC(5); break;
    case 16: runPUSHACC(6); break;
    case 17: runPUSHACC(7); break;
    case 18: runPUSHACC(code[PC++]); break;
    case 19: runPOP(code[PC++]); break;

    case 32: runAPPLY(code[PC++]); break;

    case 44: runCLOSUREREC(); break;

    case 53: runGETGLOBAL(code[PC++]); break;
    case 54: runPUSHGETGLOBAL(code[PC++]); break;

    case 57: runSETGLOBAL(code[PC++]); break;

    case 62: runMAKEBLOCK(code[PC++]); break;
    case 63: runMAKEBLOCK(1); break;
    case 64: runMAKEBLOCK(2); break;
    case 65: runMAKEBLOCK(3); break;

    case 84: runBRANCH(code[PC++]); break;
    case 85: runBRANCH(code[PC++]); break;
    case 86: runBRANCH(code[PC++]); break;

    case 93: runCCALL(1);
    case 94: runCCALL(2);
    case 95: runCCALL(3);
    case 96: runCCALL(4);
    case 97: runCCALL(5);
    case 98: runCCALL(code[PC++]);
    default:
      throw std::runtime_error("Uknonwn opcode: " + std::to_string(op));
    }
    std::cerr << "ACC: " << stack.size() << std::endl;
    printValue(*this, A, std::cout);
  }

}

void Context::runACC(uint32_t n) {
  A = stack[n];
}

void Context::runPUSH() {

}

void Context::runPUSHACC(uint32_t n) {
  stack.push_back(A);
  A = stack[n];
}

void Context::runPOP(uint32_t n) {
  for (uint32_t i = 0; i < n; ++i) {
    stack.pop_back();
  }
}

void Context::runASSIGN() {

}

void Context::runENVACC() {

}

void Context::runGETFIELD() {

}

void Context::runPUSH_RETADDR() {

}

void Context::runAPPLY(uint32_t args) {
  std::cerr << A.getSize() << std::endl;
  printValue(*this, A, std::cout);
  extraArgs = args - 1;
  exit(0);
}

void Context::runRESTART() {

}

void Context::runGRAB() {

}

void Context::runCLOSURE() {

}

void Context::runCLOSUREREC() {
  uint32_t f = code[PC++];
  uint32_t v = code[PC++];

  if (v > 0) {
    stack.push_back(A);
  }

  A = allocBlock(2 * f - 1 + v, kClosureTag);
  for (uint32_t i = 0; i < v; ++i) {
    stack.pop_back();
  }
  for (uint32_t i = 0; i < f; ++i) {
    stack.push_back(Value(0ll));
  }
  PC += f;
}

void Context::runPUSHOFFSETCLOSURE() {

}

void Context::runGETGLOBAL(uint32_t n) {
  A = global.getField(n);
}

void Context::runPUSHGETGLOBAL(uint32_t n) {
  stack.push_back(A);
  A = global.getField(n);
}

void Context::runGETGLOBALFIELD() {

}

void Context::runPUSHGETGLOBALFIELD() {

}

void Context::runSETGLOBAL(uint32_t n) {
  global.setField(n, A);
  A = Value(0ll);
}

void Context::runATOM0() {

}

void Context::runATOM() {

}

void Context::runPUSHATOM0() {

}

void Context::runPUSHATOM() {

}

void Context::runMAKEBLOCK(uint32_t n) {
  uint32_t t = code[PC++];
  Value block = allocBlock(n, t);
  block.setField(0, A);
  for (uint32_t i = 1; i < n; ++i) {
    block.setField(i, *stack.rbegin());
    stack.pop_back();
  }
  A = block;
}

void Context::runBRANCH(int32_t ofs) {
  PC += ofs - 1;
}

void Context::runBRANCHIF(int32_t ofs) {

}

void Context::runBRANCHIFNOT(int32_t ofs) {

}

void Context::runSWITCH() {

}

void Context::runPUSHTRAP() {

}

void Context::runPOPTRAP() {

}

void Context::runCHECK_SIGNALS() {

}

void Context::runPUSHCONST0() {

}

void Context::runCCALL(uint32_t n) {
  uint32_t p = code[PC++];
  if (prim_[p] == "caml_register_named_value") {
    std::cerr << "CCALL: " << p << " " << prim_[p] << std::endl;
  } else {
    throw std::runtime_error(prim_[p]);
  }
}
