#include <cassert>
#include <cstdio>
#include <dlfcn.h>
#include <librtd/elf/ELF.h>

using namespace cxx::formats;
using namespace cxx::formats::elf;

int main(int, char**) {
  Dl_info info;
  dladdr((void*) &main, &info);
  auto addr = info.dli_fbase;
  auto elf = elf::ELF::at(addr);
  assert(elf);
  auto h = elf->header();
  printf("%08x %02hhx\n", h->magic(), uint8_t(h->klass()));
  elf->eachSection([&](Section const& sec) {
    printf("  0x%012lx: (%04x)\n", uintptr_t(&sec), sec.type());
    return true;
  });
}
