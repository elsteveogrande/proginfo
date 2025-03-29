#pragma once
static_assert(__cplusplus >= 201700L, "proginfo requires C++17");

#include "../../util/Virtual.h"
#include "../Section.h"
#include "../Segment.h"
#include "../SymbolTable.h"
#include "MachO.h"

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstdlib>

namespace proginfo::binary {

class MachO64 final : public MachO {

public:
  MachO64(util::Addr addr, bool isLoaded, bool isAuxBinary)
      : MachO {addr, isLoaded, isAuxBinary} {}

  bool is64() const override {}

  uintptr_t segHeaderOffset() const override {}
  uintptr_t segHeaderSize() const override {}
  uint16_t sectionNameIndex() const override {}
  uint16_t symbolSize() const override {}

  size_t entryOffset() const override {}

  size_t segHeaderCount() const override {}
  size_t secHeaderCount() const override {}

  util::Virtual<Segment> segment(unsigned index) const override {}
  util::Virtual<Section> section(unsigned index) const override {}

  util::Virtual<SymbolTable> symTable() const override {}
};

}  // namespace proginfo::binary
