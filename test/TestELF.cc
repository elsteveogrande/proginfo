#include "Result.h"

#include <cassert>
#include <dlfcn.h>
#include <proginfo/binary/Binary.h>
#include <proginfo/util/MMap.h>

using namespace proginfo;

int main(int, char**) {
  unsigned errors = 0;

  auto check = [&](auto bin, auto sym, Result expect) {
    Result::check(&errors, bin, sym, expect);
  };

  check("test/bins/elf.64.le.exe",
        "main",
        {.elf = 1, .is64 = 1, .exe = 1, .addrs = {0x00000000000012fa}});

  check("test/bins/elf.64.le.so",
        "test",
        {.elf = 1, .is64 = 1, .lib = 1, .addrs = {0x000000000001029c}});

  check("test/bins/elf.32.be.exe",
        "main",
        {.elf = 1, .is64 = 0, .exe = 1, .addrs = {0x00010214}});

  check("test/bins/elf.32.le.exe",
        "main",
        {.elf = 1, .is64 = 0, .exe = 1, .addrs = {0x000011f8}});

  check("test/bins/elf.64.be.exe",
        "main",
        {.elf = 1, .is64 = 1, .exe = 1, .addrs = {0x0000000000304a0}});

  check("test/bins/elf.32.le.so",
        "test",
        {.elf = 1, .is64 = 0, .lib = 1, .addrs = {0x000011bc}});

  assert(!errors);
}
