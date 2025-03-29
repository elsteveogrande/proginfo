#pragma once
static_assert(__cplusplus >= 201700L, "proginfo requires C++17");

#include <cassert>
#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <string_view>

namespace proginfo::util {

struct Addr final {
  std::byte const* ptr_ {};
  size_t const size_ {};

  Addr() = default;

  Addr(void const* ptr, size_t size) : Addr((std::byte const*) ptr, size) {}

  Addr(std::byte const* ptr, size_t size)
      : ptr_((std::byte const*) ptr)
      , size_(size) {}

  Addr(Addr const& rhs) : ptr_(rhs.ptr_), size_(rhs.size_) {}

  Addr& operator=(Addr const& rhs) {
    new (this) Addr(rhs);
    return *this;
  }

  Addr operator+(size_t offset) const {
    assert(offset <= size_);
    return {ptr_ + offset, size_ - offset};
  }

  Addr trunc(size_t newSize) const {
    assert(newSize <= size_);
    return {ptr_, newSize};
  }

  std::string_view str() const { return {(char const*) ptr_, size_}; }

  uint8_t u8(size_t offset) const {
    assert(offset < size_);
    return *(((uint8_t const*) ptr_) + offset);
  }

  operator bool() const { return ptr_; }

  // Aligned or unaligned reads, adjusted for endianness.
  // TODO: Clang and GCC both produce like 40 instructions for `u32`.
  // Rewrite, or provide better hints to compiler, to help optimize this!

  uint16_t u16LE(size_t offset) const {
    auto a = uint16_t(u8(offset));
    auto b = uint16_t(u8(offset + 1));
    b <<= 8;
    return a | b;
  }

  uint16_t u16BE(size_t offset) const {
    auto a = uint16_t(u8(offset));
    auto b = uint16_t(u8(offset + 1));
    a <<= 8;
    return a | b;
  }

  uint32_t u32LE(size_t offset) const {
    auto a = uint32_t(u16LE(offset));
    auto b = uint32_t(u16LE(offset + 2));
    b <<= 16;
    return a | b;
  }

  uint32_t u32BE(size_t offset) const {
    auto a = uint32_t(u16BE(offset));
    auto b = uint32_t(u16BE(offset + 2));
    a <<= 16;
    return a | b;
  }

  uint64_t u64LE(size_t offset) const {
    auto a = uint64_t(u32LE(offset));
    auto b = uint64_t(u32LE(offset + 4));
    b <<= 32;
    return a | b;
  }

  uint64_t u64BE(size_t offset) const {
    auto a = uint64_t(u32BE(offset));
    auto b = uint64_t(u32BE(offset + 4));
    a <<= 32;
    return a | b;
  }
};

struct Bytes {
  Addr const addr_;

  explicit Bytes(Addr addr) : addr_(std::move(addr)) {}
  Bytes(Bytes const&) = default;
  Bytes(Bytes&&) = default;

  Addr baseAddr() const { return addr_; }

  virtual bool isLE() const = 0;

  uint8_t u8(size_t offset) const { return addr_.u8(offset); }

  uint16_t u16(size_t off) const {
    return isLE() ? addr_.u16LE(off) : addr_.u16BE(off);
  }

  uint32_t u32(size_t off) const {
    return isLE() ? addr_.u32LE(off) : addr_.u32BE(off);
  }

  uint64_t u64(size_t off) const {
    return isLE() ? addr_.u64LE(off) : addr_.u64BE(off);
  }
};

}  // namespace proginfo::util
