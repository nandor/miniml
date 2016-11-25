// This file is part of the miniml project.
// Licensing information can be found in the LICENSE file.
// (C) Nandor Licker. All rights reserved.

#include <iostream>
#include <memory>
#include <iomanip>

#include "miniml/BytecodeFile.h"
using namespace miniml;


int main(int argc, char **argv) {
  if (argc < 2) {
    std::cerr << "Usage: dump [path]" << std::endl;
    return EXIT_FAILURE;
  }

  try {
    for (int i = 1; i < argc; ++i) {
      BytecodeFile file(argv[i]);
      /*
      for (unsigned i = 0; i < file.getNumSections(); ++i) {
        auto section = file.getSection(i);

        std::cout << "Section " << section->getName() << std::endl;

        switch (section->getType()) {
        //case CODE: dumpCODE(static_cast<SectionCODE*>(section)); break;
        //case DATA: dumpDATA(static_cast<SectionDATA*>(section)); break;
        //case PRIM: dumpPRIM(static_cast<SectionPRIM*>(section)); break;
        //case DLLS: dumpDLLS(static_cast<SectionDLLS*>(section)); break;
        //case DLPT: dumpDLPT(static_cast<SectionDLPT*>(section)); break;
        //case DBUG: dumpDBUG(static_cast<SectionDBUG*>(section)); break;
        case CRCS: dumpCRCS(static_cast<SectionCRCS*>(section)); break;
        //case SYMB: dumpSYMB(static_cast<SectionSYMB*>(section)); break;
        }
      }
      */
    }
    return EXIT_SUCCESS;
  } catch (std::exception &e) {
    std::cerr << "[Exception]: " << e.what() << std::endl;
    return EXIT_FAILURE;
  }
}
