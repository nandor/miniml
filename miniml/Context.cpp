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
// int64
// -----------------------------------------------------------------------------
value int64_deserialize(Context &ctx, StreamReader &stream) {
  return ctx.allocInt64(stream.getInt64le());
}



CustomOperations int64_ops = {
  "_j",
  nullptr,
  nullptr,
  nullptr,
  nullptr,
  int64_deserialize,
  nullptr,
};


// -----------------------------------------------------------------------------
// Context
// -----------------------------------------------------------------------------
Context::Context()
  : heap_()
{
  custom_["_j"] = &int64_ops;
}

Context::~Context() {
}

value Context::allocInt64(int64_t i) {
  return heap_.allocInt64(i);
}

value Context::allocDouble(double v) {
  return heap_.allocDouble(v);
}

value Context::allocString(const char *str, size_t length) {
  return heap_.allocString(str, length);
}

value Context::allocBlock(size_t n, uint8_t tag) {
  return heap_.allocBlock(n, tag);
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
