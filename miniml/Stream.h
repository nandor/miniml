// This file is part of the miniml project.
// Licensing information can be found in the LICENSE file.
// (C) Nandor Licker. All rights reserved.

#pragma once

#include <cstdint>
#include <string>



namespace miniml {

/// Abstract base class for input streams.
class StreamReader {
 public:
  virtual ~StreamReader();

  // Reads bytes.
  uint8_t getUInt8() { return get<uint8_t>(); }
  int8_t getInt8() { return get<int8_t>(); }

  // Reads words in little-endian order.
  uint16_t getUInt16le() { return get<uint16_t>(); }
  uint32_t getUInt32le() { return get<uint32_t>(); }
  uint64_t getUInt64le() { return get<uint64_t>(); }
  int16_t getInt16le() { return get<int16_t>(); }
  int32_t getInt32le() { return get<int32_t>(); }
  int64_t getInt64le() { return get<int64_t>(); }

  // Reads words in big-endian order.
  uint16_t getUInt16be() { return __builtin_bswap16(getUInt16le()); }
  uint32_t getUInt32be() { return __builtin_bswap32(getUInt32le()); }
  uint64_t getUInt64be() { return __builtin_bswap64(getUInt64le()); }
  int16_t getInt16be() { return __builtin_bswap16(getInt16le()); }
  int32_t getInt32be() { return __builtin_bswap32(getInt32le()); }
  int64_t getInt64be() { return __builtin_bswap64(getInt64le()); }

  /// Reads a null-terminated string.
  std::string getString();

 protected:
  /// Reads a typed value.
  template<typename T>
  T get() {
    return *reinterpret_cast<T*>(get(sizeof(T)));
  }

  /// Virtual method that reads numBytes bytes.
  virtual void *get(size_t numBytes) = 0;
};

/// Abstract base class for output stream.
class StreamWriter {
 public:
  virtual ~StreamWriter();

  // Reads bytes.
  void putUInt8(uint8_t v) { put<uint8_t>(v); }
  void putInt8(int8_t v) { put<int8_t>(v); }

  // Reads words in little-endian order.
  void putUInt16le(uint16_t v) { put<uint16_t>(v); }
  void putUInt32le(uint32_t v) { put<uint32_t>(v); }
  void putUInt64le(uint64_t v) { put<uint64_t>(v); }
  void putInt16le(int16_t v) { put<int16_t>(v); }
  void putInt32le(int32_t v) { put<int32_t>(v); }
  void putInt64le(int64_t v) { put<int64_t>(v); }

  // Reads words in big-endian order.
  void putUInt16be(uint16_t v) { put<uint16_t>(__builtin_bswap16(v)); }
  void putUInt32be(uint32_t v) { put<uint32_t>(__builtin_bswap32(v)); }
  void putUInt64be(uint64_t v) { put<uint64_t>(__builtin_bswap64(v)); }
  void putInt16be(int16_t v) { put<int16_t>(__builtin_bswap16(v)); }
  void putInt32be(int32_t v) { put<int32_t>(__builtin_bswap32(v)); }
  void putInt64be(int64_t v) { put<int64_t>(__builtin_bswap64(v)); }

  /// Reads a null-terminated string.
  void putString(const std::string &v);

 protected:
  /// Reads a typed value.
  template<typename T>
  void put(T value) {
    put(&value, sizeof(T));
  }

  /// Virtual method that reads numBytes bytes.
  virtual void put(const void *data, size_t numBytes) = 0;
};


/// Stream that reads from memory.
class MemoryStreamReader : public StreamReader {
 public:
  /// Creates a new memory reader.
  MemoryStreamReader(const void *start, size_t size)
    : start_(start), size_(size), cursor_(0)
  {
  }

  /// Frees the memory reader.
  ~MemoryStreamReader();

 private:
  /// Reads a number of bytes from the buffer.
  void *get(size_t numBytes) override;

 private:
  /// Start of data.
  const void *start_;
  /// Length of data.
  size_t size_;
  /// Cursor address.
  size_t cursor_;
};

/// Writes data to a buffer.
class MemoryStreamWriter : public StreamWriter {
 public:
  /// Creates a new memory writer.
  MemoryStreamWriter() {
  }

  /// Frees the stream writer.
  ~MemoryStreamWriter();
};

}
