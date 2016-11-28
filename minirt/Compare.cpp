// This file is part of the miniml project.
// Licensing information can be found in the LICENSE file.
// (C) Nandor Licker. All rights reserved.

#include "miniml/Context.h"
using namespace miniml;

extern "C" value caml_greaterequal(
    Context &ctx,
    value v1,
    value v2)
{
  if (val_is_int64(v1) && val_is_int64(v2)) {
    return val_int64(val_to_int64(v1) >= val_to_int64(v2));
  }
  assert(!"not implemented");
}
