#pragma once
static_assert(__cplusplus >= 201700L, "librtd requires C++17");

#include "../util/Bytes.h"
#include "../util/Virtual.h"

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <string_view>

namespace librtd::binary {

struct Symbol : util::Bytes {
  static util::Virtual<Symbol> at(uintptr_t addr);
  static util::Virtual<Symbol> at(void* addr) { return at(uintptr_t(addr)); }
  explicit Symbol(uintptr_t addr) : util::Bytes {addr} {}
  virtual ~Symbol() = default;

  virtual uintptr_t addr() const = 0;
  virtual std::string_view name() const = 0;
};

}  // namespace librtd::binary
