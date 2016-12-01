// This file is part of the miniml project.
// Licensing information can be found in the LICENSE file.
// (C) Nandor Licker. All rights reserved.

#include "miniml/Context.h"
using namespace miniml;



// -----------------------------------------------------------------------------
// int32
// -----------------------------------------------------------------------------
value int32_deserialize(Context &, StreamReader &);
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

value int32_deserialize(Context &ctx, StreamReader &stream) {
  auto c = ctx.allocCustom(&int32_ops, sizeof(int32_t));
  *val_to_custom<int32_t>(c) = stream.getInt32be();
  return c;
}



// -----------------------------------------------------------------------------
// int64
// -----------------------------------------------------------------------------
value int64_deserialize(Context &, StreamReader &);
void  int64_print(Context &, value, std::ostream &);
CustomOperations int64_ops = {
  "_j",
  nullptr,
  nullptr,
  nullptr,
  nullptr,
  int64_deserialize,
  int64_print,
  nullptr,
};

value int64_deserialize(Context &ctx, StreamReader &stream) {
  auto c = ctx.allocCustom(&int64_ops, sizeof(int64_t));
  *val_to_custom<int64_t>(c) = stream.getInt64be();
  return c;
}

void int64_print(Context &, value val, std::ostream &os) {
  os << *val_to_custom<int64_t>(val);
}

extern "C" value caml_int64_float_of_bits(
    Context &ctx,
    value vi)
{
  union { double dbl_val; int64_t int_val; } v;
  v.int_val = *val_to_custom<int64_t>(vi);
  return ctx.allocDouble(v.dbl_val);
}


// -----------------------------------------------------------------------------
// nativeint
// -----------------------------------------------------------------------------
value nativeint_deserialize(Context &, StreamReader &);
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

value nativeint_deserialize(Context &ctx, StreamReader &stream) {
  switch (stream.getUInt8()) {
  case 1: {
    auto c = ctx.allocCustom(&nativeint_ops, sizeof(int32_t));
    *val_to_custom<int32_t>(c) = stream.getInt32be();
    return c;
  }
  case 2: {
    auto c = ctx.allocCustom(&nativeint_ops, sizeof(int64_t));
    *val_to_custom<int64_t>(c) = stream.getInt64be();
    return c;
  }
  default: {
    throw std::runtime_error("Cannot deserialize nativeint");
  }
  }
}

extern "C" value caml_nativeint_shift_left(
    Context &,
    value v1,
    value v2)
{
  return val_int64(val_to_int64(v1) << val_to_int64(v2));
}

extern "C" value caml_nativeint_sub(
    Context &,
    value v1,
    value v2)
{
  return val_int64(val_to_int64(v1) - val_to_int64(v2));
}

// -----------------------------------------------------------------------------
// Numeric stuff
// -----------------------------------------------------------------------------
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
// Tagged Ints
// -----------------------------------------------------------------------------
extern "C" value caml_int_compare(
    Context &,
    value v1,
    value v2)
{
  return val_int64((v1 > v2) - (v1 < v2));
}
