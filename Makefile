all: test

test: test_binfile test_debug

test_osx: 

test_linux:	test_linux_TestELF_clang test_linux_TestELF_gcc
	test_linux_TestELF_clang
	test_linux_TestELF_gcc

test_linux_TestELF_clang: test/TestELF.cc
	clang++ @compile_flags.txt -Og -g1 -o $@ $<

test_linux_TestELF_gcc: test/TestELF.cc
	g++ @compile_flags.txt -Og -g1 -o $@ $<
