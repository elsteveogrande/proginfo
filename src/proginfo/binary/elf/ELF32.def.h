#pragma once
static_assert(__cplusplus >= 201700L, "proginfo requires C++17");

#include "ELF32.h"
#include "Section32.h"
#include "Segment32.h"
#include "SymbolTable32.h"

namespace proginfo::binary {

inline util::Virtual<Segment> ELF32::segment(unsigned index) const {
  util::Virtual<Segment> ret;
  auto addr = baseAddr() + segHeaderOffset() + (index * segHeaderSize());
  ret.emplace<Segment32>(this, addr);
  return ret;
}

inline util::Virtual<Section> ELF32::section(unsigned index) const {
  util::Virtual<Section> ret;
  auto addr = baseAddr() + secHeaderOffset() + (index * secHeaderSize());
  ret.emplace<Section32>(this, addr);
  return ret;
}

inline std::string_view ELF32::sectionNames() const {
  auto index = sectionNameIndex();
  auto nameStrings = section(index);
  auto nameStringData = baseAddr() + nameStrings->fileOffset();
  return {(char const*) nameStringData.ptr_, nameStrings->size()};
}

inline util::Virtual<SymbolTable> ELF32::symTable() const {
  util::Virtual<SymbolTable> ret;
  auto symtab = Binary::section(".symtab");
  if (!symtab) { return ret; }
  auto strtab = Binary::section(".strtab");
  if (!strtab) { return ret; }
  auto symData = baseAddr() + symtab->fileOffset();
  auto symSize = symbolSize();
  assert(symtab->size() % symSize == 0);
  auto symCount = uint16_t(symtab->size() / symSize);
  auto strtabChars = baseAddr() + strtab->fileOffset();
  ret.emplace<SymbolTable32>(
      symData, symCount, symSize, isLE(), strtabChars.str());
  return ret;
}

}  // namespace proginfo::binary
