// This file is part of the miniml project.
// Licensing information can be found in the LICENSE file.
// (C) Nandor Licker. All rights reserved.

#include <unistd.h>

#include "miniml/Context.h"
using namespace miniml;



// -----------------------------------------------------------------------------
// channel
// -----------------------------------------------------------------------------
struct channel {
  int fd;
};

void channel_finalize(Context &ctx, value val) {
}


CustomOperations channel_ops {
  "_chan",
  channel_finalize,
  nullptr,
  nullptr,
  nullptr,
  nullptr,
  nullptr,
  nullptr,
};



// -----------------------------------------------------------------------------
extern "C" value caml_ml_open_descriptor_in(
    Context &ctx,
    value fd)
{
  auto vchannel = ctx.allocCustom(&channel_ops, sizeof(fd));
  auto chan = val_to_custom<channel>(vchannel);
  chan->fd = val_to_int64(fd);
  return vchannel;
}



extern "C" value caml_ml_open_descriptor_out(
    Context &ctx,
    value fd)
{
  auto vchannel = ctx.allocCustom(&channel_ops, sizeof(fd));
  auto chan = val_to_custom<channel>(vchannel);
  chan->fd = val_to_int64(fd);
  return vchannel;
}



extern "C" value caml_register_named_value(
    Context &ctx,
    value vname,
    value val)
{
  //std::cerr << "caml_register_named_value" << std::endl;
  return kUnit;
}

extern "C" value caml_ml_output(
    Context &ctx,
    value vchannel,
    value buff,
    value start,
    value length)
{
  auto chan = val_to_custom<channel>(vchannel);
  auto pos = val_to_int64(start);
  auto len = val_to_int64(length);
  auto *buf = val_to_string(buff);

  for (uint64_t i = 0; i < len; ++i) {
    write(chan->fd, &buf[i], 1);
  }
  return kUnit;
}

extern "C" value caml_ml_output_char(
    Context &ctx,
    value vchannel,
    value ch)
{
  auto chan = val_to_custom<channel>(vchannel);
  char code = val_to_int64(ch);
  write(chan->fd, &code, 1);
  return kUnit;
}

extern "C" value caml_ml_flush(
    Context &ctx,
    value vchannel)
{
  return kUnit;
}

extern "C" value caml_ml_out_channels_list(
    Context &ctx,
    value unit)
{
  value result = ctx.allocBlock(2, 0);
  val_field(result, 0) = caml_ml_open_descriptor_in(ctx, val_int64(0));
  val_field(result, 1) = kUnit;
  return result;
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
// String
// -----------------------------------------------------------------------------
extern "C" value caml_ml_string_length(
    Context &ctx,
    value s)
{
  const char *data = val_to_string(s);
  const size_t blkSize = val_size(s) * sizeof(value);
  return val_int64(blkSize - data[blkSize - 1] - 1);
}



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
