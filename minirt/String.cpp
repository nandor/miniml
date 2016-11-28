// This file is part of the miniml project.
// Licensing information can be found in the LICENSE file.
// (C) Nandor Licker. All rights reserved.

#include "miniml/Context.h"
using namespace miniml;



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


extern "C" value caml_create_bytes(
    Context &ctx,
    value length)
{
  return ctx.allocBytes(val_to_int64(length));
}

extern "C" value caml_blit_string(
    Context &ctx,
    value s1,
    value ofs1,
    value s2,
    value ofs2,
    value n)
{
  auto *ptr1 = val_to_string(s1) + val_to_int64(ofs1);
  auto *ptr2 = val_to_string(s2) + val_to_int64(ofs2);
  memmove(ptr2, ptr1, val_to_int64(n));
  return kUnit;
}
