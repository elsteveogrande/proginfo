#pragma once
static_assert(__cplusplus >= 201700L, "proginfo requires C++17");

#include "../SymbolTable.h"

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstdlib>

namespace proginfo::binary {

struct SymbolTable64 : SymbolTable {
  util::Addr base_;
  uint16_t count_;
  uint16_t symSize_;
  bool isLE_;
  std::string_view strings_;

  virtual ~SymbolTable64() = default;

  SymbolTable64(
      util::Addr base,
      uint16_t count,
      uint16_t symSize,
      bool isLE,
      std::string_view strings);

  void each(std::function<bool(Symbol const&)> cb) const override;
};

}  // namespace proginfo::binary
