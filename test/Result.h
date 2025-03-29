#include <cassert>
#include <dlfcn.h>
#include <iostream>
#include <ostream>
#include <proginfo/binary/Binary.h>
#include <proginfo/util/MMap.h>
#include <set>

using namespace proginfo;

struct Result {
  bool elf {};
  bool macho {};
  bool is64 {};
  bool isFat {};
  bool exe {};
  bool lib {};
  std::set<uintptr_t> addrs {};

  bool operator==(Result const& rhs) const {
    return (elf == rhs.elf) && (macho == rhs.macho) && (is64 == rhs.is64) and
           (isFat == rhs.isFat) and (exe == rhs.exe) && (lib == rhs.lib) &&
           (addrs == rhs.addrs);
  }

  bool operator!=(Result const& rhs) const { return !((*this) == rhs); }

  friend std::ostream& operator<<(std::ostream& os, Result const& r) {
    os << "elf:" << int(r.elf);
    os << " macho:" << int(r.macho);
    os << " is64:" << int(r.is64);
    os << " isFat:" << int(r.isFat);
    os << " exe:" << int(r.exe);
    os << " lib:" << int(r.lib);
    os << " addrs:{";
    for (auto a : r.addrs) { os << ((void*) a) << ','; }
    os << "}";
    return os;
  }

  static Result run(std::string_view path, std::string_view symbol) {
    std::cerr << "TestMachO: binary: " << path << '\n';
    proginfo::util::Alloc a;
    (void) symbol;
    util::MMap mm(path);
    mm.check();
    auto bin =
        std::move(binary::Binary::at(mm.addr_, mm.size_, false, false).check());
    std::set<uintptr_t> addrs {};
    auto st = bin->symTable();
    assert(st);
    st->each([&](auto& sym) {
      if (sym.name() == symbol) {
        addrs.insert(sym.value());
        return false;
      }
      return true;
    });
    return Result {
        bin->isELF(),
        bin->isMachO(),
        bin->is64(),
        bin->isFat(),
        bin->isExecutable(),
        bin->isSharedObject(),
        addrs};
  };

  static void check(
      unsigned* errors,
      std::string_view binary,
      std::string_view symbol,
      Result const& expect) {
    auto actual = run(binary, symbol);
    if (actual != expect) {
      ++*errors;
      std::cerr
          << "binary:  " << binary << '\n'
          << "symbol:  " << symbol << '\n'
          << "expect:  " << expect << '\n'
          << "actual:  " << actual << '\n';
    }
  };
};
