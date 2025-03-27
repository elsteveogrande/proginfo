#pragma once
static_assert(__cplusplus >= 201700L, "proginfo requires C++17");

#include "../util/Bytes.h"
#include "../util/Virtual.h"

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <string_view>

namespace proginfo::binary {

class Binary;

class Segment : public util::Bytes {
  Binary const* bin_;

public:
  virtual ~Segment() = default;
  Segment(Binary const* bin, util::Addr addr) : Bytes {addr}, bin_(bin) {}

  Segment(Segment const&) = default;
  Segment& operator=(Segment const&) = default;

  virtual bool canWrite() const = 0;
  virtual bool canExecute() const = 0;
  virtual bool loadable() const = 0;
  virtual uintptr_t virtAddr() const = 0;
  virtual uintptr_t virtSize() const = 0;
  virtual uintptr_t fileOffset() const = 0;
  virtual uintptr_t fileSize() const = 0;

  Binary const& bin() const { return *bin_; }
  virtual std::string_view name() const = 0;

  static util::Virtual<Segment> at(util::Addr addr);
};

}  // namespace proginfo::binary
