#pragma once
static_assert(__cplusplus >= 201700L, "proginfo requires C++17");

#include "../../util/Virtual.h"
#include "../Section.h"
#include "../Segment.h"
#include "../SymbolTable.h"
#include "ELF.h"

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstdlib>

namespace proginfo::binary {

class ELF32 : public ELF {
private:
  static util::Addr assertELF32At(util::Addr addr) {
    assert(addr.u32BE(0) == 0x7f454c46);
    assert(addr.u8(4) == 0x01);
    return addr;
  }

public:
  ELF32(util::Addr addr, bool isLoaded, bool isAuxBinary)
      : ELF(assertELF32At(addr), isLoaded, isAuxBinary) {}

  virtual ~ELF32() = default;

  bool is64() const override final { return false; }

  size_t entryOffset() const override { return size_t(u32(0x18)); }

  uintptr_t segHeaderOffset() const override { return u32(0x1c); }
  uintptr_t segHeaderSize() const override { return uintptr_t(u16(0x2a)); }
  size_t segHeaderCount() const override { return size_t(u16(0x2c)); }

  uintptr_t secHeaderOffset() const override { return u32(0x20); }
  uintptr_t secHeaderSize() const override { return uintptr_t(u16(0x2e)); }
  size_t secHeaderCount() const override { return size_t(u16(0x30)); }

  uint16_t sectionNameIndex() const override { return u16(0x32); }
  uint16_t symbolSize() const override { return 16; }

  util::Virtual<Segment> segment(unsigned index) const override;
  util::Virtual<Section> section(unsigned index) const override;
  std::string_view sectionNames() const;

  util::Virtual<SymbolTable> symTable() const override;
};

}  // namespace proginfo::binary
