// This file is part of the miniml project.
// Licensing information can be found in the LICENSE file.
// (C) Nandor Licker. All rights reserved.

#include <iostream>
#include <memory>
#include <iomanip>

#include "miniml/Context.h"
#include "miniml/BytecodeFile.h"
#include "miniml/Value.h"
#include "minirt/Runtime.h"
using namespace miniml;



// -----------------------------------------------------------------------------
int main(int argc, char **argv) {
  if (argc < 2) {
    std::cerr << "Usage: dump [path]" << std::endl;
    return EXIT_FAILURE;
  }


  try {
    Context ctx;
    ctx.registerOperations(&int32_ops);
    ctx.registerOperations(&int64_ops);
    ctx.registerOperations(&nativeint_ops);

    for (int i = 1; i < argc; ++i) {
      BytecodeFile file(argv[i]);
      auto result = ctx.run(file);
      if (result != kUnit) {
        printValue(ctx, result, std::cerr);
      }
    }
    return EXIT_SUCCESS;
  } catch (std::exception &e) {
    std::cerr << "[Exception]: " << e.what() << std::endl;
    return EXIT_FAILURE;
  }
}
