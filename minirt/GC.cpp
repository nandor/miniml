// This file is part of the miniml project.
// Licensing information can be found in the LICENSE file.
// (C) Nandor Licker. All rights reserved.

#include "miniml/Context.h"
using namespace miniml;



// -----------------------------------------------------------------------------
// Garbage Collector Interface
// -----------------------------------------------------------------------------
extern "C" value caml_get_minor_free(
    Context &ctx,
    value unit)
{
  return val_int64(0x100000);
}

extern "C" value caml_gc_full_major(
    Context &ctx,
    value unit)
{
  return kUnit;
}
