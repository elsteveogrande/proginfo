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
  bool exe {};
  bool lib {};
  bool le {};
  std::set<uintptr_t> addrs {};

  bool operator==(Result const& rhs) const {
    return (elf == rhs.elf) && (macho == rhs.macho) && (exe == rhs.exe) &&
           (lib == rhs.lib) && (le == rhs.le) && (addrs == rhs.addrs);
  }

  bool operator!=(Result const& rhs) const { return !((*this) == rhs); }

  friend std::ostream& operator<<(std::ostream& os, Result const& r) {
    os << "elf:" << int(r.elf);
    os << " macho:" << int(r.macho);
    os << " exe:" << int(r.exe);
    os << " lib:" << int(r.lib);
    os << " le:" << int(r.le);
    os << " addrs:{";
    for (auto a : r.addrs) { os << ((void*) a) << ','; }
    os << "}";
    return os;
  }
};

int main(int, char**) {
  auto run = [&](std::string_view path, std::string_view symbol) {
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
        bin->isExecutable(),
        bin->isSharedObject(),
        bin->isLE(),
        addrs};
  };

  unsigned errors = 0;
  auto check =
      [&](std::string_view binary, std::string_view symbol, Result const& expect) mutable {
        auto actual = run(binary, symbol);
        if (actual != expect) {
          ++errors;
          std::cerr
              << "binary:  " << binary << '\n'
              << "symbol:  " << symbol << '\n'
              << "expect:  " << expect << '\n'
              << "actual:  " << actual << '\n';
        }
      };

  check("test/bins/elf.64.le.exe",
        "main",
        {.elf = 1, .exe = 1, .le = 1, .addrs = {0x00000000000012fa}});

  check("test/bins/elf.64.le.so",
        "test",
        {.elf = 1, .lib = 1, .le = 1, .addrs = {0x000000000001029c}});

  check("test/bins/elf.32.be.exe",
        "main",
        {.elf = 1, .exe = 1, .le = 0, .addrs = {0x00010214}});

  check("test/bins/elf.32.le.exe",
        "main",
        {.elf = 1, .exe = 1, .le = 1, .addrs = {0x000011f8}});

  check("test/bins/elf.64.be.exe",
        "main",
        {.elf = 1, .exe = 1, .le = 0, .addrs = {0x00000000000304a0}});

  check("test/bins/elf.32.le.so",
        "test",
        {.elf = 1, .lib = 1, .le = 1, .addrs = {0x000011bc}});

  // {.path = "test/bins/macho.64.le.exe",
  //  .macho = 1,
  //  .exe = 1,
  //  .le = 0,
  //  .name = "_main",
  //  .a64 = {0x00000001000002e0}},

  // {.path = "test/bins/macho.64.le.dylib",
  //  .macho = 1,
  //  .lib = 1,
  //  .le = 0,
  //  .name = "_test",
  //  .a64 = {0x0000000000000290}},

  // {.path = "test/bins/macho.fat.be.exe",
  //  .macho = 1,
  //  .exe = 1,
  //  .le = 0,
  //  .name = "_main",
  //  .a32 = {0x0000e020}, /* armv7, armv7s (same addr for both) */
  //  .a64 = {0x0000000100009e94} /* arm64 */},

  // {.path = "test/bins/macho.fat.le.dylib",
  //  .macho = 1,
  //  .lib = 1,
  //  .le = 0,
  //  .name = "_libSystem_initializer",
  //  .a32 = {0x000019fd}, /* i386 */
  //  .a64 = {0x0000000000001a1a} /* x86_64 */},

  assert(!errors);
}
