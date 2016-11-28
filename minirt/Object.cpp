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

extern "C" value caml_obj_dup(
    Context &ctx,
    value arg)
{
  size_t size = val_size(arg);
  if (size == 0) {
    return arg;
  }

  uint8_t tag = val_tag(arg);
  value ret = ctx.allocBlock(size, tag);
  if (tag >= kNoScanTag) {
    memcpy(val_ptr(ret), val_ptr(arg), size * sizeof(value));
  } else {
    for (uint32_t i = 0; i < size; ++i) {
      val_field(ret, i) = val_field(arg, i);
    }
  }
  return ret;
}
