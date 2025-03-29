#pragma once
static_assert(__cplusplus >= 201700L, "proginfo requires C++17");

#include "../Symbol.h"

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstdlib>

namespace proginfo::binary {

struct Symbol64 final : Symbol {
  std::string_view strings_;
  bool const isLE_;

  virtual ~Symbol64() = default;

  Symbol64(util::Addr addr, unsigned symSize, std::string_view strings, bool isLE)
      : Symbol(addr.trunc(symSize))
      , strings_(strings)
      , isLE_(isLE) {}

  bool isLE() const override { return isLE_; }

  std::string_view name() const override {
    auto ret = strings_.substr(nameIndex());
    return {ret.data(), strlen(ret.data())};
  }

  uint32_t nameIndex() const { return u32(0); }
  uint8_t info() const { return u8(4); }
  uint8_t other() const { return u8(5); }
  uint16_t secIndex() const { return u16(6); }
  uintptr_t value() const override { return u64(8); }
};

}  // namespace proginfo::binary
