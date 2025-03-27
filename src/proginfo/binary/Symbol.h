#pragma once
static_assert(__cplusplus >= 201700L, "proginfo requires C++17");

#include "../util/Bytes.h"

#include <cassert>
#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <string_view>

namespace proginfo::binary {

struct Symbol : util::Bytes {
  virtual ~Symbol() = default;
  explicit Symbol(util::Addr addr) : util::Bytes {addr} {}

  virtual uintptr_t value() const = 0;
  virtual std::string_view name() const = 0;
};

}  // namespace proginfo::binary
