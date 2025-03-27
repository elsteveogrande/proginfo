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
class Segment;

class Section : public util::Bytes {
  Binary const* bin_;

public:
  virtual ~Section() = default;

  Section(Binary const* bin, util::Addr addr) : util::Bytes {addr}, bin_(bin) {}

  Section(Section const&) = default;
  Section& operator=(Section const&) = default;

  virtual bool canWrite() const = 0;
  virtual bool canExecute() const = 0;
  virtual bool loadable() const = 0;
  virtual size_t size() const = 0;
  virtual uintptr_t virtAddr() const = 0;
  virtual uintptr_t fileOffset() const = 0;

  Binary const& bin() const { return *bin_; }
  virtual util::Virtual<Segment> segment() const = 0;
  virtual std::string_view name() const = 0;

  static util::Virtual<Section> at(util::Addr addr);
};

}  // namespace proginfo::binary
