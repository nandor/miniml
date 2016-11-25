// This file is part of the miniml project.
// Licensing information can be found in the LICENSE file.
// (C) Nandor Licker. All rights reserved.

#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace miniml {
class Section;
class BytecodeFile;

/// Enumeration of section types.
enum SectionType {
  CODE,
  DATA,
  PRIM,
  DLLS,
  DLPT,
  DBUG,
  CRCS,
  SYMB
};

/// Wrapper over a bytecode file.
class BytecodeFile {
 public:
  BytecodeFile(const std::string &path);
  virtual ~BytecodeFile();

  unsigned getNumSections() const { return sections_.size(); }

  /// Finds a section by index.
  Section *getSection(unsigned i) const;

  /// Finds a section by type.
  Section *getSection(SectionType type) const;

 private:
  /// Pointer to the mmapped region.
  uint8_t *start_;
  /// Size of the bytecode file.
  size_t size_;
  /// File descriptor.
  int fd_;
  /// Index-to-section mapping.
  std::vector<Section*> sections_;

  friend class SectionStrings;
};

/// Wrapper over a section.
class Section {
 public:
  Section(
      const BytecodeFile *file,
      const uint8_t *start,
      const size_t size,
      SectionType type);

  virtual ~Section();

  /// Returns the type of the section.
  SectionType getType() const { return type_; }

  /// Returns the name of the section.
  const char *getName() const {
    switch (type_) {
    case CODE: return "CODE";
    case DATA: return "DATA";
    case PRIM: return "PRIM";
    case DLLS: return "DLLS";
    case DLPT: return "DLPT";
    case DBUG: return "DBUG";
    case CRCS: return "CRCS";
    case SYMB: return "SYMB";
    }
  }

  /// Returns a pointer to section data.
  const uint8_t *getData() { return start_; }

  /// Returns the size of the section.
  size_t getSize() const { return size_; }

 protected:
  /// Reference to the bytecode file containing the section.
  const BytecodeFile *file_;
  /// Start of the section.
  const uint8_t *start_;
  /// Size of the section.
  size_t size_;
  /// Name of the section (4 bytes).
  SectionType type_;
};

} // namespace miniml
