#pragma once
static_assert(__cplusplus >= 201700L, "proginfo requires C++17");

#include "../../util/Bytes.h"
#include "../../util/Virtual.h"

#include <cassert>
#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <functional>

namespace proginfo::binary {

class Segment;
class Section;
class SymbolTable;
class ELF32;
class ELF64;

class Binary : public util::Bytes {
  bool isLoaded_ {};
  bool isAuxBinary_ {};

public:
  virtual ~Binary() = default;

  inline static util::Virtual<Binary>
  at(void const* ptr, size_t size, bool isLoaded, bool isAuxBinary);

  explicit Binary(util::Addr addr, bool isLoaded, bool isAuxBinary)
      : util::Bytes {addr}
      , isLoaded_(isLoaded)
      , isAuxBinary_(isAuxBinary) {}

  Binary(Binary const&) = default;
  Binary& operator=(Binary const& rhs) = default;

  ELF32 const& asELF32() const;
  ELF64 const& asELF64() const;

  virtual bool isExecutable() const = 0;
  virtual bool isSharedObject() const = 0;
  virtual bool isLoaded() const final { return isLoaded_; }
  virtual bool isAuxBinary() const final { return isAuxBinary_; }

  virtual bool isELF() const = 0;
  virtual bool isMachO() const = 0;
  virtual bool is64() const = 0;

  virtual size_t entryOffset() const = 0;

  virtual size_t segHeaderCount() const = 0;
  virtual size_t secHeaderCount() const = 0;

  virtual util::Virtual<Segment> segment(unsigned index) const = 0;
  virtual util::Virtual<Section> section(unsigned index) const = 0;

  virtual void eachSegment(std::function<bool(Segment const&)> cb) const final;
  virtual util::Virtual<Segment> segment(std::string_view name) const final;

  virtual void eachSection(std::function<bool(Section const&)> cb) const final;
  virtual util::Virtual<Section> section(std::string_view name) const final;

  virtual util::Virtual<SymbolTable> symTable() const = 0;
};

}  // namespace proginfo::binary
