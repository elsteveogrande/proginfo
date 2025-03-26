#pragma once
static_assert(__cplusplus >= 201700L, "librtd requires C++17");

#include "binary/Binary.h"
#include "binary/detail/ELF32.h"
#include "binary/detail/ELF64.h"
#include "util/Virtual.h"

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cstring>

namespace librtd {

struct Context {
  util::Virtual<binary::Binary>
  binaryAt(uintptr_t addr, bool isLoaded, bool isAuxBinary) {
    util::Virtual<binary::Binary> ret;
    auto* magic = (char const*) addr;
    if (!memcmp(magic, "\x7f\x45\x4c\x46\x01", 5)) {
      ret.emplace<binary::ELF32>(addr, isLoaded, isAuxBinary);
    } else if (!memcmp(magic, "\x7f\x45\x4c\x46\x02", 5)) {
      ret.emplace<binary::ELF64>(addr, isLoaded, isAuxBinary);
    }
    return ret;
  }
};

}  // namespace librtd
