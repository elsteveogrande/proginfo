#pragma once
static_assert(__cplusplus >= 201700L, "proginfo requires C++17");

#include "../../util/Virtual.h"
#include "../Section.h"
#include "../Segment.h"
#include "ELF64.h"

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstdlib>

namespace proginfo::binary {

struct Section64 final : Section {
  virtual ~Section64() = default;
  Section64(Binary const* bin, util::Addr addr) : Section(bin, addr) {}

  uint32_t nameIndex() const { return u32(0); }
  uint32_t type() const { return u32(4); }
  uint64_t flags() const { return u64(8); }

  size_t size() const override { return u64(0x20); }
  uintptr_t virtAddr() const override { return u64(0x10); }
  uintptr_t fileOffset() const override { return u64(0x18); }

  bool canWrite() const override { return flags() & 0x00000001; }
  bool canExecute() const override { return flags() & 0x00000004; }
  bool loadable() const override { return flags() & 0x00000002; }

  util::Virtual<Segment> segment() const override;

  std::string_view name() const override {
    auto elf64 = bin().asELF64();
    auto names = elf64.sectionNames();
    auto index = nameIndex();
    auto ret = names.substr(index);
    return {ret.data(), strlen(ret.data())};
  }
};

}  // namespace proginfo::binary
