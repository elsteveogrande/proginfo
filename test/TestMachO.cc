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

  check("test/bins/macho.64.le.exe",
        "_main",
        {.macho = 1, .is64 = 1, .exe = 1, .addrs = {0x00000001000002e0}});

  // {.path = "test/bins/macho.64.le.dylib",
  //  .macho = 1,
  //  .lib = 1,
  //  .name = "_test",
  //  .a64 = {0x0000000000000290}},

  // {.path = "test/bins/macho.fat.be.exe",
  //  .macho = 1,
  //  .exe = 1,
  //  .name = "_main",
  //  .a32 = {0x0000e020}, /* armv7, armv7s (same addr for both) */
  //  .a64 = {0x0000000100009e94} /* arm64 */},

  // {.path = "test/bins/macho.fat.le.dylib",
  //  .macho = 1,
  //  .lib = 1,
  //  .name = "_libSystem_initializer",
  //  .a32 = {0x000019fd}, /* i386 */
  //  .a64 = {0x0000000000001a1a} /* x86_64 */},

  assert(!errors);
}
