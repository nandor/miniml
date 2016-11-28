// This file is part of the miniml project.
// Licensing information can be found in the LICENSE file.
// (C) Nandor Licker. All rights reserved.

#include "miniml/Context.h"
using namespace miniml;



// -----------------------------------------------------------------------------
// Object
// -----------------------------------------------------------------------------
extern "C" value caml_make_vect(
    Context &ctx,
    value len,
    value init)
{
  size_t size = val_to_int64(len);
  uint8_t tag = val_tag(init);
  value ret = ctx.allocBlock(size, tag);
  for (size_t i = 0; i < size; ++i) {
    val_field(ret, i) = init;
  }
  return ret;
}

extern "C" value caml_array_get_addr(
    Context &ctx,
    value array,
    value index)
{
  return val_field(array, val_to_int64(index));
}


extern "C" value caml_array_set_addr(
    Context &ctx,
    value array,
    value index,
    value newval)
{
  val_field(array, val_to_int64(index)) = newval;
  return kUnit;
}
