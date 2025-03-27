#pragma once
static_assert(__cplusplus >= 201700L, "proginfo requires C++17");

#include "../Section.h"
#include "../Segment.h"

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstdlib>

namespace proginfo::binary {

struct Segment32 final : Segment {
  virtual ~Segment32() = default;

  Segment32(Binary const* bin, util::Addr addr) : Segment(bin, addr) {}

  uint32_t type() const { return u32(0); }
  uint32_t flags() const { return u32(0x18); }

  bool canWrite() const override { return flags() & 0x00000002; }
  bool canExecute() const override { return flags() & 0x00000001; }
  bool loadable() const override { return type() & 0x00000001; }

  uintptr_t virtAddr() const override { return u32(0x08); }
  uintptr_t virtSize() const override { return u32(0x14); }
  uintptr_t fileOffset() const override { return u32(0x04); }
  uintptr_t fileSize() const override { return u32(0x10); }

  std::string_view name() const override { return "TODO"; }
};

}  // namespace proginfo::binary
