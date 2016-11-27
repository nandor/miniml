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
