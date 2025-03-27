#pragma once
static_assert(__cplusplus >= 201700L, "proginfo requires C++17");

#include "ELF64.h"
#include "Section64.h"
#include "Segment64.h"
#include "SymbolTable64.h"

namespace proginfo::binary {

inline util::Virtual<Segment> ELF64::segment(unsigned index) const {
  util::Virtual<Segment> ret;
  auto addr = baseAddr() + segHeaderOffset() + (index * segHeaderSize());
  ret.emplace<Segment64>(this, addr);
  return ret;
}

inline util::Virtual<Section> ELF64::section(unsigned index) const {
  util::Virtual<Section> ret;
  auto addr = baseAddr() + secHeaderOffset() + (index * secHeaderSize());
  ret.emplace<Section64>(this, addr);
  return ret;
}

inline std::string_view ELF64::sectionNames() const {
  auto index = sectionNameIndex();
  auto nameStrings = section(index);
  auto nameStringData = baseAddr() + nameStrings->fileOffset();
  return {(char const*) nameStringData.ptr_, nameStrings->size()};
}

inline util::Virtual<SymbolTable> ELF64::symTable() const {
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
  ret.emplace<SymbolTable64>(
      symData, symCount, symSize, isLE(), strtabChars.str());
  return ret;
}

}  // namespace proginfo::binary
