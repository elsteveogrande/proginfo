#pragma once
static_assert(__cplusplus >= 201700L, "proginfo requires C++17");

#include "../elf/ELF32.h"
#include "../elf/ELF64.h"
#include "Binary.h"

namespace proginfo::binary {

inline util::Virtual<Segment> Binary::segment(std::string_view name) const {
  util::Virtual<Segment> ret;
  auto n = unsigned(segHeaderCount());
  for (unsigned i = 0; i < n; i++) {
    auto seg = segment(i);
    if (seg->name() == name) {
      ret = std::move(seg);
      break;
    }
  }
  return ret;
}

inline util::Virtual<Section> Binary::section(std::string_view name) const {
  util::Virtual<Section> ret;
  auto n = unsigned(secHeaderCount());
  for (unsigned i = 0; i < n; i++) {
    auto sec = section(i);
    if (sec->name() == name) {
      ret = std::move(sec);
      break;
    }
  }
  return ret;
};

inline void Binary::eachSegment(std::function<bool(Segment const&)> cb) const {
  auto n = unsigned(segHeaderCount());
  for (unsigned i = 0; i < n; i++) {
    auto seg = segment(i);
    if (!cb(*seg)) { break; }
  }
}

inline void Binary::eachSection(std::function<bool(Section const&)> cb) const {
  auto n = unsigned(secHeaderCount());
  for (unsigned i = 0; i < n; i++) {
    auto sec = section(i);
    if (!cb(*sec)) { break; }
  }
}

inline util::Virtual<binary::Binary>
Binary::at(void const* ptr, size_t size, bool isLoaded, bool isAuxBinary) {
  util::Virtual<binary::Binary> ret;
  auto buf = (uint8_t*) ptr;
  if (!memcmp(buf, "\x7f\x45\x4c\x46", 4)) {
    bool isLE = (buf[5] == 0x01);
    util::Addr addr {ptr, size, isLE};
    if (buf[4] == 0x02) {
      ret.emplace<binary::ELF64>(addr, isLoaded, isAuxBinary);
    } else if (buf[4] == 0x01) {
      ret.emplace<binary::ELF32>(addr, isLoaded, isAuxBinary);
    }
  }
  return ret;
}

inline ELF32 const& Binary::asELF32() const {
  assert(isELF());
  assert(!is64());
  auto* ret = dynamic_cast<ELF32 const*>(this);
  assert(ret);
  return *ret;
}

inline ELF64 const& Binary::asELF64() const {
  assert(isELF());
  assert(is64());
  auto* ret = dynamic_cast<ELF64 const*>(this);
  assert(ret);
  return *ret;
}

}  // namespace proginfo::binary
