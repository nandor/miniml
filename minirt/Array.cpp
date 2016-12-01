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
  value ret = ctx.allocBlock(size, 0);
  for (size_t i = 0; i < size; ++i) {
    val_field(ret, i) = init;
  }
  return ret;
}

extern "C" value caml_array_get_addr(
    Context &,
    value array,
    value index)
{
  return val_field(array, val_to_int64(index));
}


extern "C" value caml_array_set_addr(
    Context &,
    value array,
    value index,
    value newval)
{
  val_field(array, val_to_int64(index)) = newval;
  return kUnit;
}

extern "C" value caml_array_unsafe_get(
    Context &,
    value array,
    value index)
{
  size_t sz = val_size(array);
  int64_t idx = val_to_int64(index);
  if (idx < 0 || static_cast<int64_t>(sz) <= idx) {
    throw std::runtime_error("caml_array_unsafe_get");
  }
  return val_field(array, val_to_int64(index));
}

extern "C" value caml_array_get(
    Context &,
    value array,
    value index)
{
  // TODO: make it safe.
  return val_field(array, val_to_int64(index));
}

extern "C" value caml_array_blit(
    Context &,
    value a1,
    value ofs1,
    value a2,
    value ofs2,
    value n)
{
  if (val_tag(a2) == kDoubleArrayTag) {
    throw std::runtime_error("caml_array_blit");
  }

  value *src = &val_field(a1, val_to_int64(ofs1));
  value *dst = &val_field(a2, val_to_int64(ofs2));
  for (int64_t i = 0; i < val_to_int64(n); ++i) {
    dst[i] = src[i];
  }
  return kUnit;
}
