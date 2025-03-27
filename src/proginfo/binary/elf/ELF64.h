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

class ELF64 : public ELF {
private:
  static util::Addr assertELF64At(util::Addr addr) {
    assert(addr.u32BE(0) == 0x7f454c46);
    assert(addr.u8(4) == 0x02);
    return addr;
  }

public:
  ELF64(util::Addr addr, bool isLoaded, bool isAuxBinary)
      : ELF(assertELF64At(addr), isLoaded, isAuxBinary) {}

  virtual ~ELF64() = default;

  bool is64() const override final { return true; }

  size_t entryOffset() const override { return size_t(u64(0x18)); }

  uintptr_t segHeaderOffset() const override { return u64(0x20); }
  uintptr_t segHeaderSize() const override { return uintptr_t(u16(0x36)); }
  size_t segHeaderCount() const override { return uintptr_t(u16(0x38)); }

  uintptr_t secHeaderOffset() const override { return u64(0x28); }
  uintptr_t secHeaderSize() const override { return uintptr_t(u16(0x3a)); }
  size_t secHeaderCount() const override { return uintptr_t(u16(0x3c)); }

  uint16_t sectionNameIndex() const override { return u16(0x3e); }
  uint16_t symbolSize() const override { return 24; }

  util::Virtual<Segment> segment(unsigned index) const override;
  util::Virtual<Section> section(unsigned index) const override;
  std::string_view sectionNames() const;

  util::Virtual<SymbolTable> symTable() const override;
};

}  // namespace proginfo::binary
