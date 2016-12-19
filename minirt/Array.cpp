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
  if (val_tag(init) == kDoubleTag) {
    assert(!"caml_make_vect");
  } else {
    size_t size = val_to_int64(len);
    value ret = ctx.allocBlock(size, 0);
    for (size_t i = 0; i < size; ++i) {
      val_field(ret, i) = init;
    }
    return ret;
  }
}

extern "C" value caml_array_get_float(
    Context &ctx,
    value array,
    value index)
{
  return ctx.allocDouble(val_to_dbl(val_field(array, val_to_int64(index))));
}

extern "C" value caml_array_set_float(
    Context &,
    value array,
    value index,
    value val)
{
  val_field(array, val_to_int64(index)) = dbl_to_val(val_to_double(val));
  return kUnit;
}

extern "C" value caml_make_float_vect(
    Context &ctx,
    value len)
{
  size_t size = val_to_int64(len);
  if (size == 0) {
    return ctx.allocAtom(0);
  } else {
    return ctx.allocBlock(size, kDoubleArrayTag);
  }
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

extern "C" value caml_array_unsafe_set(
    Context &,
    value array,
    value index,
    value val)
{
  if (val_tag(array) == kDoubleArrayTag) {
    assert(false);
  } else {
    val_field(array, val_to_int64(index)) = val;
  }
  return kUnit;
}

extern "C" value caml_array_unsafe_get(
    Context &ctx,
    value array,
    value index)
{
  if (val_tag(array) == kDoubleArrayTag) {
    return ctx.allocDouble(val_to_dbl(val_field(array, val_to_int64(index))));
  } else {
    return val_field(array, val_to_int64(index));
  }
}

extern "C" value caml_array_get(
    Context &ctx,
    value array,
    value index)
{
  if (val_tag(array) == kDoubleArrayTag) {
    return ctx.allocDouble(val_to_dbl(val_field(array, val_to_int64(index))));
  } else {
    return val_field(array, val_to_int64(index));
  }
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
    assert(!"caml_array_blit");
  } else {
    value *src = &val_field(a1, val_to_int64(ofs1));
    value *dst = &val_field(a2, val_to_int64(ofs2));
    for (int64_t i = 0; i < val_to_int64(n); ++i) {
      dst[i] = src[i];
    }
    return kUnit;
  }
}
