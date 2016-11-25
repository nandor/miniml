// This file is part of the miniml project.
// Licensing information can be found in the LICENSE file.
// (C) Nandor Licker. All rights reserved.

#pragma once

namespace miniml {
class Heap;

class Context {
 public:


 private:
  /// Memory Manager.
  Heap *heap;
};

} // namespace miniml
