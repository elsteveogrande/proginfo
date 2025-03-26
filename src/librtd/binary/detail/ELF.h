#pragma once
static_assert(__cplusplus >= 201700L, "librtd requires C++17");

#include "../../util/Virtual.h"
#include "../Binary.h"

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cstring>

namespace librtd::binary {
inline namespace detail {

struct ELFIdent {
  char magic[4];       // "\x7f", "ELF"
  uint8_t classByte;   // 0x01=32bit, 0x02=64bit
  uint8_t endianByte;  // 0x01=LE, 0x02=BE
  uint8_t elfVersion;  // 0x01 expected
  uint8_t osABI;
  uint8_t abiVersion;
  uint8_t _padding[7];
};
static_assert(sizeof(ELFIdent) == 16);

class ELF : public Binary {

protected:
  ELF(uintptr_t addr, bool isLoaded, bool isAuxBinary)
      : Binary {addr, isLoaded, isAuxBinary} {}

  virtual ~ELF() = default;

public:
  bool isELF() const override final { return true; }
  bool isMachO() const override final { return false; }

  bool isLE() const override { return ident().endianByte == 0x01; }

  ELFIdent const& ident() const { return *(ELFIdent const*) (baseAddr()); }

  bool isExecutable() const override final { return u16(0x10) == 0x0002; }
  bool isSharedObject() const override final { return u16(0x10) == 0x0003; }

  virtual bool eachSegment(std::function<bool(Segment const&)> cb) const = 0;
  virtual bool eachSection(std::function<bool(Section const&)> cb) const = 0;
  virtual util::Virtual<SymbolTable> symTable() const = 0;
};

}  // namespace detail
}  // namespace librtd::binary
