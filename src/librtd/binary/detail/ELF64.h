#pragma once
static_assert(__cplusplus >= 201700L, "librtd requires C++17");

#include "../../util/Virtual.h"
#include "ELF.h"

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cstring>

namespace librtd::binary {
inline namespace detail {

class ELF64 : public ELF {
private:
  static uintptr_t assertELF64At(uintptr_t baseAddr) {
    assert(!memcmp((void*) baseAddr, "\x7f\x45\x4c\x46\x02", 5));
    return baseAddr;
  }

public:
  ELF64(uintptr_t addr, bool isLoaded, bool isAuxBinary)
      : ELF(assertELF64At(addr), isLoaded, isAuxBinary) {}

  virtual ~ELF64() = default;

  bool is64() const override final { return true; }

  virtual bool eachSegment(std::function<bool(Segment const&)> cb) const = 0;
  virtual bool eachSection(std::function<bool(Section const&)> cb) const = 0;
  virtual util::Virtual<SymbolTable> symTable() const = 0;
};

}  // namespace detail
}  // namespace librtd::binary
