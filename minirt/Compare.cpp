// This file is part of the miniml project.
// Licensing information can be found in the LICENSE file.
// (C) Nandor Licker. All rights reserved.

#include "miniml/Context.h"
using namespace miniml;



// -----------------------------------------------------------------------------
// Compare
// -----------------------------------------------------------------------------
extern "C" value caml_greaterequal(
    Context &,
    value v1,
    value v2)
{
  if (val_is_int64(v1) && val_is_int64(v2)) {
    return val_int64(val_to_int64(v1) >= val_to_int64(v2));
  }
  assert(!"not implemented");
}

value compare(value v1, value v2) {
  // TODO: handle cycles.
  if (val_is_int64(v1) && val_is_int64(v2)) {
    return val_to_int64(v1) - val_to_int64(v2);
  }

  auto tag1 = val_tag(v1), tag2 = val_tag(v2);
  if (tag1 != tag2) {
    return tag1 - tag2;
  }

  switch (tag1) {
  case kStringTag:
    assert(!"kStringTag");
  case kDoubleTag: {
    double d1 = val_to_double(v1), d2 = val_to_double(v2);
    if (d1 < d2) {
      return -1;
    } else if (d1 > d2) {
      return + 1;
    } else {
      return 0;
    }
  }
  case kDoubleArrayTag:
    assert(!"kDoubleArrayTag");
  default: {
    int64_t sz1 = val_size(v1), sz2 = val_size(v2);
    if (sz1 != sz2) {
      return sz1 - sz2;
    }

    for (int64_t i = 0; i < sz1; ++i) {
      if (int r = compare(val_field(v1, i), val_field(v2, i))) {
        return r;
      }
    }

    return 0;
  }
  }
}

extern "C" value caml_compare(
    Context &,
    value v1,
    value v2,
    value)
{
  return val_int64(compare(v1, v2));
}
