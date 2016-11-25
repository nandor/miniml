// This file is part of the miniml project.
// Licensing information can be found in the LICENSE file.
// (C) Nandor Licker. All rights reserved.

#include <iostream>
#include <memory>
#include <iomanip>

#include "miniml/Context.h"
#include "miniml/Value.h"
#include "miniml/BytecodeFile.h"
using namespace miniml;



// -----------------------------------------------------------------------------
static void dumpData(Section *section) {
  Context ctx;
  MemoryStreamReader stream(section->getData(), section->getSize());
  printValue(ctx, getValue(ctx, stream), std::cout);
}



// -----------------------------------------------------------------------------
static void dumpStrings(Section *section) {
  MemoryStreamReader stream(section->getData(), section->getSize());
  while (!stream.eof()) {
    std::cout << "  " << stream.getString() << std::endl;
  }
}


// -----------------------------------------------------------------------------
int main(int argc, char **argv) {
  if (argc < 2) {
    std::cerr << "Usage: dump [path]" << std::endl;
    return EXIT_FAILURE;
  }

  try {
    for (int i = 1; i < argc; ++i) {
      BytecodeFile file(argv[i]);
      for (unsigned i = 0; i < file.getNumSections(); ++i) {
        auto section = file.getSection(i);

        std::cout << "Section " << section->getName() << std::endl;

        switch (section->getType()) {
        case CODE:
          // TODO: code.
          break;
        case PRIM: case DLLS: case DLPT:
          dumpStrings(section);
          break;
        case DATA: case CRCS:
          dumpData(section);
          break;
        case DBUG: case SYMB:
          break;
        }
      }
    }
    return EXIT_SUCCESS;
  } catch (std::exception &e) {
    std::cerr << "[Exception]: " << e.what() << std::endl;
    return EXIT_FAILURE;
  }
}
