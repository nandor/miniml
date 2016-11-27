// This file is part of the miniml project.
// Licensing information can be found in the LICENSE file.
// (C) Nandor Licker. All rights reserved.

#include "miniml/Context.h"
using namespace miniml;



// -----------------------------------------------------------------------------
// Object
// -----------------------------------------------------------------------------
static int64_t oo_last_id = 0ll;


extern "C" value caml_set_oo_id(
    Context &ctx,
    value obj)
{
  val_field(obj, 1) = val_int64(oo_last_id);
  oo_last_id += 1;
  return obj;
}


extern "C" value caml_fresh_oo_id(
    Context &ctx,
    value v)
{
  value val = val_int64(oo_last_id);
  oo_last_id += 1;
  return val;
}
