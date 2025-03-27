HEADERS = src/proginfo/binary/Binary.h \
					src/proginfo/binary/Section.h \
					src/proginfo/binary/Segment.h \
					src/proginfo/binary/Symbol.h \
					src/proginfo/binary/SymbolTable.h \
					src/proginfo/binary/detail/Binary.def.h \
					src/proginfo/binary/detail/Binary.h \
					src/proginfo/binary/elf/ELF.h \
					src/proginfo/binary/elf/ELF32.def.h \
					src/proginfo/binary/elf/ELF32.h \
					src/proginfo/binary/elf/ELF64.def.h \
					src/proginfo/binary/elf/ELF64.h \
					src/proginfo/debug/DWARF.h \
					src/proginfo/util/Alloc.h \
					src/proginfo/util/Bytes.h \
					src/proginfo/util/Cleanup.h \
					src/proginfo/util/MMap.h \
					src/proginfo/util/Virtual.h \

TESTS = build/TestELF.exe \

.PHONY: test

test: buildtests
	$(foreach TEST,$(TESTS),$(TEST))

buildtests: $(TESTS)

build/%.exe: test/%.cc $(HEADERS)
	mkdir -p build
	clang++ @compile_flags.txt -fsanitize=address,undefined -O0 -g -o $@ $<

clean:
	rm -rf build
