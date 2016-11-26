// This file is part of the miniml project.
// Licensing information can be found in the LICENSE file.
// (C) Nandor Licker. All rights reserved.

#include "miniml/Context.h"
using namespace miniml;

extern "C" value caml_int64_float_of_bits(
    Context &ctx,
    value vi)
{
  union { double dbl_val; int64_t int_val; } v;
  v.int_val = val_to_int64(vi);
  return ctx.allocDouble(v.dbl_val);
}

extern "C" value caml_ml_open_descriptor_in(
    Context &ctx,
    value fd)
{
  std::cerr << "caml_ml_open_descriptor_in" << std::endl;
  exit(0);
  return 1ull;
}

extern "C" value caml_ml_open_descriptor_out(
    Context &ctx,
    value fd)
{
  std::cerr << "caml_ml_open_descriptor_out" << std::endl;
  exit(0);
  return 1ull;
}

extern "C" value caml_register_named_value(
    Context &ctx,
    value vname,
    value val)
{
  std::cerr << "caml_register_named_value" << std::endl;
  return kUnit;
}

extern "C" value caml_set_oo_id(
    Context &ctx,
    value obj)
{
  static int64_t oo_last_id = 0ll;
  val_field(obj, 1) = val_int64(oo_last_id);
  oo_last_id += 1;
  return obj;
}
