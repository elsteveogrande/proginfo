#pragma once
static_assert(__cplusplus >= 201700L, "proginfo requires C++17");

#include "../Section.h"
#include "../Segment.h"
#include "../detail/Binary.h"

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <mach-o/loader.h>

namespace proginfo::binary {

class MachO : public Binary {
protected:
  virtual ~MachO() = default;

  bool isLE_;

  static bool isMagicLE(std::byte const* magic) {
    return (!memcmp((void*) magic, "\xce\xfa\xed\xfe", 4)) ||
           (!memcmp((void*) magic, "\xcf\xfa\xed\xfe", 4)) ||
           (!memcmp((void*) magic, "\xca\xfe\xba\xbe", 4));
  }

  MachO(util::Addr addr, bool isLoaded, bool isAuxBinary)
      : Binary {addr, isLoaded, isAuxBinary}
      , isLE_(isMagicLE(addr.ptr_)) {}

public:
  bool isELF() const override final { return false; }
  bool isMachO() const override final { return true; }

  // from mach-o/loader.h:
  enum Type {
    _MH_OBJECT = 0x1,       // relocatable object file
    _MH_EXECUTE = 0x2,      // demand paged executable file
    _MH_FVMLIB = 0x3,       // fixed VM shared library file
    _MH_CORE = 0x4,         // core file
    _MH_PRELOAD = 0x5,      // preloaded executable file
    _MH_DYLIB = 0x6,        // dynamically bound shared library
    _MH_DYLINKER = 0x7,     // dynamic link editor
    _MH_BUNDLE = 0x8,       // dynamically bound bundle file
    _MH_DYLIB_STUB = 0x9,   // shared library stub
    _MH_DSYM = 0xa,         // companion file w/ only debug sections
    _MH_KEXT_BUNDLE = 0xb,  // x86_64 kexts
    _MH_FILESET = 0xc,      // a file composed of other Mach-Os
    _MH_GPU_EXECUTE = 0xd,  // gpu program
    _MH_GPU_DYLIB = 0xe,    // gpu support functions
  };

  enum Flag {
    _MH_NOUNDEFS = 0x1,
    _MH_INCRLINK = 0x2,
    _MH_DYLDLINK = 0x4,
    _MH_BINDATLOAD = 0x8,
    _MH_PREBOUND = 0x10,
    _MH_SPLIT_SEGS = 0x20,
    _MH_LAZY_INIT = 0x40,
    _MH_TWOLEVEL = 0x80,
    _MH_FORCE_FLAT = 0x100,
    _MH_NOMULTIDEFS = 0x200,
    _MH_NOFIXPREBINDING = 0x400,
    _MH_PREBINDABLE = 0x800,
    _MH_ALLMODSBOUND = 0x1000,
    _MH_CANONICAL = 0x4000,
    _MH_WEAK_DEFINES = 0x8000,
    _MH_BINDS_TO_WEAK = 0x10000,
    _MH_ALLOW_STACK_EXECUTION = 0x20000,
    _MH_ROOT_SAFE = 0x40000,
    _MH_SETUID_SAFE = 0x80000,
    _MH_NO_REEXPORTED_DYLIBS = 0x100000,
    _MH_PIE = 0x200000,
    _MH_DEAD_STRIPPABLE_DYLIB = 0x400000,
    _MH_HAS_TLV_DESCRIPTORS = 0x800000,
    _MH_NO_HEAP_EXECUTION = 0x1000000,
    _MH_APP_EXTENSION_SAFE = 0x02000000,
    _MH_NLIST_OUTOFSYNC_WITH_DYLDINFO = 0x04000000,
    _MH_SIM_SUPPORT = 0x08000000,
    _MH_IMPLICIT_PAGEZERO = 0x10000000,
    _MH_DYLIB_IN_CACHE = 0x80000000,
  };

  // https://github.com/aidansteele/osx-abi-macho-file-format-reference?tab=readme-ov-file#mach_header_64

  uint32_t magic() const { return u32(0); }
  uint32_t cputype() const { return u32(4); }
  uint32_t cpusubtype() const { return u32(8); }
  uint32_t filetype() const { return u32(12); }
  uint32_t ncmds() const { return u32(16); }
  uint32_t sizeofcmds() const { return u32(20); }
  uint32_t flags() const { return u32(24); }

  Type type() const { return (Type) filetype(); }

  bool isLE() const override final { return isLE_; }

  bool isFat() const override final {
    // TODO: support this
    // auto m = magic();
    // return m == 0xcafebabe || m == 0xbebafeca;
    return false;
  }

  bool isExecutable() const override final {
    switch (type()) {
    case Type::_MH_EXECUTE:
    case Type::_MH_GPU_EXECUTE: return true;
    default:                    return false;
    }
  }

  bool isSharedObject() const override final {
    switch (type()) {
    case Type::_MH_DYLIB: return true;
    default:              return false;
    }
  }

  virtual uintptr_t segHeaderOffset() const = 0;
  virtual uintptr_t segHeaderSize() const = 0;
  virtual uint16_t sectionNameIndex() const = 0;
  virtual uint16_t symbolSize() const = 0;
};

}  // namespace proginfo::binary
