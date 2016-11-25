// This file is part of the miniml project.
// Licensing information can be found in the LICENSE file.
// (C) Nandor Licker. All rights reserved.

#include "miniml/Context.h"
using namespace miniml;

extern "C" {

Value caml_register_named_value(Context &ctx, Value name, Value val) {
  return Value(0ll);
}

}
