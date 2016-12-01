// This file is part of the miniml project.
// Licensing information can be found in the LICENSE file.
// (C) Nandor Licker. All rights reserved.

#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

#include "miniml/BytecodeFile.h"
using namespace miniml;

/// Magic string at the end of bytecode files.
static const char MAGIC[] = "Caml1999X011";

/// Enumeration of section names.
static const char* SECTIONS[] = {
  "CODE",
  "DATA",
  "PRIM",
  "DLLS",
  "DLPT",
  "DBUG",
  "CRCS",
  "SYMB",
};



struct [[gnu::packed]] MTrailer {
  uint32_t numSections;
  char magic[12];
};

struct [[gnu::packed]] MSection {
  char name[4];
  uint32_t size;
};



BytecodeFile::BytecodeFile(const std::string &path)
  : start_(nullptr)
  , size_(0)
  , fd_(-1)
{
  // MMap the file.
  if ((fd_ = open(path.c_str(), O_RDONLY)) < 0) {
    throw std::runtime_error("File '" + path + "' does not exist.");
  }
  struct stat buf;
  if (stat(path.c_str(), &buf) < 0) {
    close(fd_);
    throw std::runtime_error("Cannot stat '" + path + "'.");
  }
  size_ = buf.st_size;
  void *start = mmap(nullptr, size_, PROT_READ, MAP_PRIVATE, fd_, 0);
  if (start == nullptr) {
    close(fd_);
    throw std::runtime_error("Cannot mmap '" + path + "'.");
  }
  start_ = static_cast<uint8_t*>(start);

  // Read the trailer.
  if (size_ < sizeof(MTrailer)) {
    throw std::runtime_error("Invalid file: too short");
  }
  auto *t = reinterpret_cast<MTrailer*>(start_ + size_ - sizeof(MTrailer));
  if (memcmp(t->magic, MAGIC, sizeof(MAGIC))) {
    throw std::runtime_error("Invalid file: wrong magic");
  }
  sections_.resize(__builtin_bswap32(t->numSections));

  // Check if all headers are there.
  size_t sectionOffset = sizeof(MSection) * sections_.size() + sizeof(MTrailer);
  if (size_ < sectionOffset) {
    throw std::runtime_error("Invalid file: missing header");
  }

  // Read section start addresses.
  size_t sectionAddr = size_ - sectionOffset;
  auto *sections = reinterpret_cast<MSection*>(start_ + sectionAddr);
  for (int i = sections_.size() - 1; i >= 0; --i) {
    // Decode the section header.
    auto &section = sections[i];
    uint32_t size = __builtin_bswap32(section.size);
    if (size > sectionAddr) {
      throw std::runtime_error("Invalid file: bad section offset");
    }
    sectionAddr -= size;

    // Compute the section address.
    const uint8_t *addr = start_ + sectionAddr;

    // Decode the section type & ensure it is unique.
    SectionType type;
    bool typeFound = false;
    for (unsigned i = 0; i < sizeof(SECTIONS) / sizeof(SECTIONS[0]); ++i) {
      if (!memcmp(section.name, SECTIONS[i], 4)) {
        type = static_cast<SectionType>(i);
        typeFound = true;
        break;
      }
    }
    if (!typeFound) {
      throw std::runtime_error("Invalid file: bad section type");
    }
    for (int j = sections_.size() - 1; j > i; --j) {
      if (sections_[j]->getType() == type) {
        throw std::runtime_error("Invalid file: duplicate section type");
      }
    }

    // Create the section.
    sections_[i] = new Section(this, addr, size, type);
  }
}

BytecodeFile::~BytecodeFile() {
  // Delete sections.
  for (auto &section : sections_) {
    delete section;
  }

  // Close the file.
  if (start_) {
    munmap(start_, size_);
  }
  if (fd_ < 0) {
    close(fd_);
  }
}

Section *BytecodeFile::getSection(unsigned i) const {
  if (i >= sections_.size()) {
    throw std::runtime_error("Section index out of range");
  }
  return sections_[i];
}

Section *BytecodeFile::getSection(SectionType type) const {
  for (const auto &section : sections_) {
    if (section->getType() == type) {
      return section;
    }
  }
  throw std::runtime_error("Section type not found");
}



Section::Section(
    const BytecodeFile *file,
    const uint8_t *start,
    const size_t size,
    SectionType type)
  : file_(file)
  , start_(start)
  , size_(size)
  , type_(type)
{
}

Section::~Section() {
}
