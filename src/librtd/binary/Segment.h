#pragma once
static_assert(__cplusplus >= 201700L, "librtd requires C++17");

#include "../util/Bytes.h"
#include "../util/Virtual.h"

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <functional>
#include <string_view>

namespace librtd::binary {

struct Section;

struct Segment : util::Bytes {
  static util::Virtual<Segment> at(uintptr_t addr);
  static util::Virtual<Segment> at(void* addr) { return at(uintptr_t(addr)); }
  explicit Segment(uintptr_t addr) : util::Bytes {addr} {}
  virtual ~Segment() = default;

  virtual bool canWrite() const = 0;
  virtual bool canExecute() const = 0;
  virtual std::string_view name() const = 0;
  virtual bool eachSegment(std::function<bool(Section const&)> cb) const = 0;
};

}  // namespace librtd::binary
