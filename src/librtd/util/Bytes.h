#pragma once
static_assert(__cplusplus >= 201700L, "librtd requires C++17");

#include <cassert>
#include <cstddef>
#include <cstdlib>
#include <cstring>

namespace librtd::util {

struct Bytes {
  uintptr_t const addr_;

  explicit Bytes(uintptr_t const addr) : addr_(addr) {}
  virtual ~Bytes() = default;

  auto u8(size_t offset) const { return *(uint8_t*) (addr_ + offset); }

  // Aligned or unaligned reads, adjusted for endianness.
  // TODO: Clang and GCC both produce like 40 instructions for `u32`.
  // Rewrite, or provide better hints to compiler, to help optimize this!

  auto u16LE(size_t offset) const {
    auto a = uint16_t(u8(offset));
    auto b = uint16_t(u8(offset + 1));
    b <<= 8;
    return a | b;
  }

  auto u16BE(size_t offset) const {
    auto a = uint16_t(u8(offset));
    auto b = uint16_t(u8(offset + 1));
    a <<= 8;
    return a | b;
  }

  auto u16(size_t offset) const {
    return isLE() ? u16LE(offset) : u16BE(offset);
  }

  auto u32LE(size_t offset) const {
    auto a = uint32_t(u16LE(offset));
    auto b = uint32_t(u16LE(offset + 2));
    b <<= 16;
    return a | b;
  }

  auto u32BE(size_t offset) const {
    auto a = uint32_t(u16LE(offset));
    auto b = uint32_t(u16LE(offset + 2));
    a <<= 16;
    return a | b;
  }

  auto u32(size_t offset) const {
    return isLE() ? u32LE(offset) : u32BE(offset);
  }

  auto u64LE(size_t offset) const {
    auto a = uint64_t(u16LE(offset));
    auto b = uint64_t(u16LE(offset + 4));
    b <<= 32;
    return a | b;
  }

  auto u64BE(size_t offset) const {
    auto a = uint64_t(u16LE(offset));
    auto b = uint64_t(u16LE(offset + 4));
    a <<= 32;
    return a | b;
  }

  auto u64(size_t offset) const {
    return isLE() ? u64LE(offset) : u64BE(offset);
  }

  uintptr_t baseAddr() const { return addr_; }

  virtual bool isLE() const = 0;
};

}  // namespace librtd::util
