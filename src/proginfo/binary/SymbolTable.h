#pragma once
static_assert(__cplusplus >= 201700L, "proginfo requires C++17");

#include "Symbol.h"

#include <cassert>
#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <functional>

namespace proginfo::binary {

class SymbolTable {
public:
  virtual ~SymbolTable() = default;
  virtual void each(std::function<bool(Symbol const&)> cb) const = 0;
};

}  // namespace proginfo::binary
