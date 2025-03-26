#pragma once
static_assert(__cplusplus >= 201700L, "librtd requires C++17");

#include "../util/Bytes.h"
#include "../util/Virtual.h"
#include "Symbol.h"

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <functional>

namespace librtd::binary {

struct SymbolTable : util::Bytes {
  static util::Virtual<SymbolTable> at(uintptr_t addr);
  static util::Virtual<SymbolTable> at(void* addr) { return at(uintptr_t(addr)); }
  explicit SymbolTable(uintptr_t addr) : util::Bytes {addr} {}
  virtual ~SymbolTable() = default;

  virtual void each(std::function<bool(Symbol const&)> cb) = 0;
};

}  // namespace librtd::binary
