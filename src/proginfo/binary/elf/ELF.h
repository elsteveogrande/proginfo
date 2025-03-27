#pragma once
static_assert(__cplusplus >= 201700L, "proginfo requires C++17");

#include "../detail/Binary.h"

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cstring>

namespace proginfo::binary {

class ELF : public Binary {

protected:
  virtual ~ELF() = default;

  ELF(util::Addr addr, bool isLoaded, bool isAuxBinary)
      : Binary {addr, isLoaded, isAuxBinary} {}

public:
  bool isELF() const override final { return true; }
  bool isMachO() const override final { return false; }

  uint8_t endianByte() const { return u8(5); }

  bool isExecutable() const override {
    if (u16(0x10) == 0x0002) { return true; }
    if (u16(0x10) == 0x0003) { return entryOffset(); }
    return false;
  }

  bool isSharedObject() const override {
    if (u16(0x10) == 0x0003) { return !entryOffset(); }
    return false;
  }

  virtual uintptr_t segHeaderOffset() const = 0;
  virtual uintptr_t segHeaderSize() const = 0;
  virtual uintptr_t secHeaderOffset() const = 0;
  virtual uintptr_t secHeaderSize() const = 0;
  virtual uint16_t sectionNameIndex() const = 0;
  virtual uint16_t symbolSize() const = 0;
};

}  // namespace proginfo::binary
