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
  fsync(val_to_custom<channel>(vchannel)->fd);
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
