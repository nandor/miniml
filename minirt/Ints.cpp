// This file is part of the miniml project.
// Licensing information can be found in the LICENSE file.
// (C) Nandor Licker. All rights reserved.

#include "miniml/Context.h"
using namespace miniml;



// -----------------------------------------------------------------------------
// int32
// -----------------------------------------------------------------------------
value int32_deserialize(Context &ctx, StreamReader &stream) {
  return ctx.allocInt64(stream.getInt32be());
}



// -----------------------------------------------------------------------------
// int64
// -----------------------------------------------------------------------------
value int64_deserialize(Context &ctx, StreamReader &stream) {
  return ctx.allocInt64(stream.getInt64le());
}



// -----------------------------------------------------------------------------
// nativeint
// -----------------------------------------------------------------------------
value nativeint_deserialize(Context &ctx, StreamReader &stream) {
  switch (stream.getUInt8()) {
  case 1: return ctx.allocInt64(stream.getInt32be());
  case 2: return ctx.allocInt64(stream.getInt64le());
  default: throw std::runtime_error("Cannot deserialize nativeint");
  }
}



// -----------------------------------------------------------------------------
// Numeric stuff
// -----------------------------------------------------------------------------
extern "C" value caml_int64_float_of_bits(
    Context &ctx,
    value vi)
{
  union { double dbl_val; int64_t int_val; } v;
  v.int_val = val_to_int64(vi);
  return ctx.allocDouble(v.dbl_val);
}

extern "C" value caml_format_float(
    Context &ctx,
    value fmt,
    value arg)
{
  // TODO: improve this.
  char *str = nullptr;
  asprintf(&str, val_to_string(fmt), val_to_double(arg));
  value v = ctx.allocString(str, strlen(str));
  free(str);
  return v;
}

extern "C" value caml_format_int(
    Context &ctx,
    value fmt,
    value arg)
{
  // TODO: improve this.
  char *str = nullptr;
  asprintf(&str, val_to_string(fmt), val_to_int64(arg));
  value v = ctx.allocString(str, strlen(str));
  free(str);
  return v;
}



// -----------------------------------------------------------------------------
// Custom Operations
// -----------------------------------------------------------------------------
CustomOperations int32_ops = {
  "_i",
  nullptr,
  nullptr,
  nullptr,
  nullptr,
  int32_deserialize,
  nullptr,
  nullptr,
};


CustomOperations int64_ops = {
  "_j",
  nullptr,
  nullptr,
  nullptr,
  nullptr,
  int64_deserialize,
  nullptr,
  nullptr,
};


CustomOperations nativeint_ops = {
  "_n",
  nullptr,
  nullptr,
  nullptr,
  nullptr,
  nativeint_deserialize,
  nullptr,
  nullptr,
};
