// This file is part of the miniml project.
// Licensing information can be found in the LICENSE file.
// (C) Nandor Licker. All rights reserved.

#include "miniml/Context.h"
using namespace miniml;

extern "C" value caml_get_minor_free(
    Context &ctx,
    value unit)
{
  return val_int64(0x100000);
}
