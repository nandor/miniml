// This file is part of the miniml project.
// Licensing information can be found in the LICENSE file.
// (C) Nandor Licker. All rights reserved.

#include "miniml/BytecodeFile.h"
#include "miniml/Context.h"
#include "miniml/Stream.h"
#include "miniml/Value.h"
#include "miniml/Interpreter.h"
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
  Value global = getValue(*this, codeStream);

  // Decode the code.
  auto codeSection = file.getSection(CODE);
  auto code = reinterpret_cast<const uint32_t *>(codeSection->getData());

  // Decode the names of linked methods.
  std::vector<std::string> primNames;
  auto primSection = file.getSection(PRIM);
  MemoryStreamReader primStream(primSection->getData(), primSection->getSize());
  while (!primStream.eof()) {
    primNames.push_back(primStream.getString());
  }

  Interpreter(*this, code, global).run();
}
