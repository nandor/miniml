// This file is part of the miniml project.
// Licensing information can be found in the LICENSE file.
// (C) Nandor Licker. All rights reserved.

#pragma once

#include "miniml/Value.h"

namespace miniml {

class Heap {
 public:

  Value allocInt();
  Value allocString(size_t length);
  Value allocBlock(size_t length, uint8_t tag);

 private:
};

}
