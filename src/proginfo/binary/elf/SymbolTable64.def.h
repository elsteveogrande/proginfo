#pragma once
static_assert(__cplusplus >= 201700L, "proginfo requires C++17");

#include "Symbol64.h"
#include "SymbolTable64.h"

namespace proginfo::binary {

inline SymbolTable64::SymbolTable64(
    util::Addr base, uint16_t count, uint16_t symSize, bool isLE, std::string_view strings)
    : base_(base)
    , count_(count)
    , symSize_(symSize)
    , isLE_(isLE)
    , strings_(strings) {}

inline void SymbolTable64::each(std::function<bool(Symbol const&)> cb) const {
  for (size_t i = 0; i < count_; i++) {
    auto symEntryAddr = base_ + (i * symSize_);
    Symbol64 sym(symEntryAddr, symSize_, strings_, isLE_);
    if (!cb(sym)) { break; }
  }
}

}  // namespace proginfo::binary
