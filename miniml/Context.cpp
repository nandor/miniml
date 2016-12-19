// This file is part of the miniml project.
// Licensing information can be found in the LICENSE file.
// (C) Nandor Licker. All rights reserved.

#include <dlfcn.h>

#include "miniml/BytecodeFile.h"
#include "miniml/Context.h"
#include "miniml/Stream.h"
#include "miniml/Value.h"
#include "miniml/Interpreter.h"
using namespace miniml;



// -----------------------------------------------------------------------------
// Context
// -----------------------------------------------------------------------------
Context::Context()
  : heap_()
{
  for (size_t i = 0; i < 256; ++i) {
    atom_[i] = allocBlock(0, i);
  }
}

Context::~Context() {
}

Value Context::allocInt64(int64_t i) {
  return heap_.allocInt64(i);
}

Value Context::allocDouble(double v) {
  return heap_.allocDouble(v);
}

Value Context::allocBytes(size_t length) {
  return heap_.allocBytes(length);
}

Value Context::allocString(const char *str, size_t length) {
  return heap_.allocString(str, length);
}

Value Context::allocBlock(size_t n, uint8_t tag) {
  return heap_.allocBlock(n, tag);
}

Value Context::allocCustom(CustomOperations *op, size_t size) {
  return heap_.allocCustom(op, size);
}

Value Context::allocAtom(uint8_t id) {
  return atom_[id];
}

void Context::registerOperations(CustomOperations *value) {
  custom_[value->identifier] = value;
}

CustomOperations *Context::getOperations(const std::string &name) {
  auto it = custom_.find(name);
  if (it == custom_.end()) {
    throw std::runtime_error("Operations for '" + name + "' not found.");
  }
  return it->second;
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
  std::vector<std::string> primSyms;
  auto primSection = file.getSection(PRIM);
  MemoryStreamReader primStream(primSection->getData(), primSection->getSize());
  while (!primStream.eof()) {
    primSyms.push_back(primStream.getString());
  }

  // Helper to link methods from a dylib.
  std::vector<void *> prim(primSyms.size(), nullptr);
  auto link = [&prim, &primSyms](const char *lib) {
    auto handle = dlopen(lib, 0);
    if (handle == nullptr) {
      return;
    }

    for (size_t i = 0; i < primSyms.size(); ++i) {
      if (auto fn = dlsym(handle, primSyms[i].c_str())) {
        prim[i] = fn;
      }
    }
    dlclose(handle);
  };

  // Link methods from the runtime.
  link(nullptr);

  // Run the interpreter.
  Interpreter(*this, code, global, prim).run();
}
