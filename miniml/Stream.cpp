// This file is part of the miniml project.
// Licensing information can be found in the LICENSE file.
// (C) Nandor Licker. All rights reserved.

#include "miniml/Stream.h"
using namespace miniml;



// -----------------------------------------------------------------------------
// StreamReader
// -----------------------------------------------------------------------------
StreamReader::~StreamReader() {
}

std::string StreamReader::getString() {
  std::string buffer;
  while (auto chr = getInt8()) {
    buffer.push_back(chr);
  }
  return buffer;
}



// -----------------------------------------------------------------------------
// StreamWriter
// -----------------------------------------------------------------------------
StreamWriter::~StreamWriter() {
}

void StreamWriter::putString(const std::string &v) {
  put(reinterpret_cast<const void*>(v.c_str()), v.size());
}



// -----------------------------------------------------------------------------
// MemoryStreamReader
// -----------------------------------------------------------------------------
MemoryStreamReader::~MemoryStreamReader() {
}

void *MemoryStreamReader::get(size_t numBytes) {
  if (cursor_ >= size_ || cursor_ + numBytes > size_) {
    throw std::runtime_error("End of stream reached.");
  }
  auto addr = reinterpret_cast<uintptr_t>(start_) + cursor_;
  cursor_ += numBytes;
  return reinterpret_cast<void*>(addr);
}



// -----------------------------------------------------------------------------
// MemoryStreamWriter
// -----------------------------------------------------------------------------
MemoryStreamWriter::~MemoryStreamWriter() {
}
