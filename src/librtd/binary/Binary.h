#pragma once
static_assert(__cplusplus >= 201700L, "librtd requires C++17");

#include "../util/Bytes.h"
#include "../util/Virtual.h"
#include "Section.h"
#include "Segment.h"
#include "SymbolTable.h"

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <functional>

namespace librtd::binary {

class Binary : public util::Bytes {
  bool const isLoaded_ {};
  bool const isAuxBinary_ {};

public:
  static util::Virtual<Binary> at(uintptr_t addr);
  static util::Virtual<Binary> at(void* addr) { return at(uintptr_t(addr)); }
  virtual ~Binary() = default;

  explicit Binary(uintptr_t addr, bool isLoaded, bool isAuxBinary)
      : util::Bytes {addr}
      , isLoaded_(isLoaded)
      , isAuxBinary_(isAuxBinary) {}

  virtual bool isExecutable() const = 0;
  virtual bool isSharedObject() const = 0;
  virtual bool isLoaded() const final { return isLoaded_; }
  virtual bool isAuxBinary() const final { return isAuxBinary_; }

  virtual bool isELF() const = 0;
  virtual bool isMachO() const = 0;
  virtual bool is64() const = 0;

  virtual bool eachSegment(std::function<bool(Segment const&)> cb) const = 0;
  virtual bool eachSection(std::function<bool(Section const&)> cb) const = 0;
  virtual util::Virtual<SymbolTable> symTable() const = 0;
};

}  // namespace librtd::binary
