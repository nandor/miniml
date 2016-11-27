// This file is part of the miniml project.
// Licensing information can be found in the LICENSE file.
// (C) Nandor Licker. All rights reserved.

#include "miniml/Context.h"
using namespace miniml;



// -----------------------------------------------------------------------------
// System
// -----------------------------------------------------------------------------
extern "C" value caml_sys_get_argv(
    Context &ctx,
    value unit)
{
  Value ret = ctx.allocBlock(2, 0);
  ret.setField(0, ctx.allocString("miniml", 6));
  ret.setField(1, ctx.allocBlock(0, 0));
  return ret;
}

extern "C" value caml_sys_get_config(
    Context &ctx,
    value unit)
{
  Value ret = ctx.allocBlock(3, 0);
  ret.setField(0, ctx.allocString("OSX", 3));
  ret.setField(1, ctx.allocInt64(8 * sizeof(value)));
  ret.setField(2, kFalse);
  return ret;
}

extern "C" value caml_sys_const_backend_type(
    Context &ctx,
    value unit)
{
  return val_int64(1);
}

extern "C" value caml_sys_const_big_endian(
    Context &ctx,
    Value unit)
{
  return kFalse;
}

extern "C" value caml_sys_const_word_size(
    Context &ctx,
    Value unit)
{
  return val_int64(8 * sizeof(value));
}

extern "C" value caml_sys_const_int_size(
    Context &ctx,
    Value unit)
{
  return val_int64(sizeof(value) * 8 - 1);
}

extern "C" value caml_sys_const_max_wosize(
    Context &ctx,
    Value unit)
{
  return val_int64((1ull << 54ull) - 1ull);
}

extern "C" value caml_sys_const_ostype_cygwin(
    Context &ctx,
    Value unit)
{
  return kFalse;
}

extern "C" value caml_sys_const_ostype_unix(
    Context &ctx,
    Value unit)
{
  return kTrue;
}

extern "C" value caml_sys_const_ostype_win32(
    Context &ctx,
    Value unit)
{
  return kFalse;
}

