// This file is part of the miniml project.
// Licensing information can be found in the LICENSE file.
// (C) Nandor Licker. All rights reserved.

#include "miniml/Context.h"
using namespace miniml;



// -----------------------------------------------------------------------------
// Float operations
// -----------------------------------------------------------------------------
extern "C" value caml_eq_float(
    Context &,
    value lhs,
    value rhs)
{
  return val_int64(val_to_double(lhs) == val_to_double(rhs));
}
