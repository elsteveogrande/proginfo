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

struct Segment;

struct Section : util::Bytes {
  static util::Virtual<Section> at(uintptr_t addr);
  static util::Virtual<Section> at(void* addr) { return at(uintptr_t(addr)); }
  explicit Section(uintptr_t addr) : util::Bytes {addr} {}
  virtual ~Section() = default;

  virtual bool canWrite() const = 0;
  virtual bool canExecute() const = 0;
  virtual std::string_view name() const = 0;
  virtual util::Virtual<Segment> segment() const = 0;
};

}  // namespace librtd::binary
