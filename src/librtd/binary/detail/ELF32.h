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

class ELF32 : public ELF {
private:
  static char const* assertELF32At(char const* baseAddr) {
    assert(!memcmp(baseAddr, "\x7f\x45\x4c\x46\x01", 5));
    return baseAddr;
  }

public:
  ELF32(uintptr_t addr, bool isLoaded, bool isAuxBinary)
      : ELF {addr, isLoaded, isAuxBinary} {}

  virtual ~ELF32() = default;

  bool is64() const override final { return false; }

  virtual bool eachSegment(std::function<bool(Segment const&)> cb) const = 0;
  virtual bool eachSection(std::function<bool(Section const&)> cb) const = 0;
  virtual util::Virtual<SymbolTable> symTable() const = 0;
};

}  // namespace detail
}  // namespace librtd::binary
