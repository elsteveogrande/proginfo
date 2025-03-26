# CLANG=/opt/homebrew/Cellar/llvm@18/18.1.8/bin/clang
CLANG=/opt/llvm/bin/clang
CFLAGS=-std=c23 -Os -g -gsplit-dwarf -nostdinc -nostdlib -fuse-ld=lld \

# Ideally we would test the power-set of:
# {elf,macho} * {32,64,fat} * {LE,BE} * {executable,sharedlib}
# But we probably don't need every *possible* combination.
testbins: \
	elf.32.le.exe \
	elf.32.be.exe \
	elf.64.le.exe \
	elf.64.be.exe \
	macho.64.le.exe \
 	macho.32.le.exe \
	macho.fat.be.exe \
	macho.fat.le.dylib \
	macho.64.le.dylib \
	elf.32.le.so \
	elf.64.le.so \

# elf.32.le.exe: ELF 32-bit LSB pie executable
elf.32.le.exe: simple.exe.c
	$(CLANG) $(CFLAGS) \
		-o $@ $< --target=i386-pc-linux-gnu
	file $@

# elf.32.be.exe: elf.32.be.exe: ELF 32-bit MSB pie executable, PowerPC or cisco 4500
elf.32.be.exe: simple.exe.c
	$(CLANG) $(CFLAGS) \
		-o $@ $< --target=ppc-pc-linux-gnu
	file $@

# elf.64.le.exe: ELF 64-bit LSB pie executable
elf.64.le.exe: simple.exe.c
	$(CLANG) $(CFLAGS) \
		-o $@ $< --target=x86_64-pc-linux-gnu
	file $@

# elf.64.be.exe: ELF 64-bit MSB pie executable
elf.64.be.exe: simple.exe.c
	$(CLANG) $(CFLAGS) \
		-o $@ $< --target=ppc64-pc-linux-gnu
	file $@

# macho.64.le.exe:
macho.64.le.exe: simple.exe.c
	$(CLANG) $(CFLAGS) \
		-o $@ $< --target=aarch64-apple-darwin
	file $@

# elf.32.le.so: ELF 32-bit LSB shared object
elf.32.le.so: simple.lib.c
	$(CLANG) $(CFLAGS) \
		-shared -fPIC -o $@ $< --target=i386-pc-linux-gnu
	file $@

# elf.64.le.so: ELF 64-bit LSB shared object, ARM aarch64
elf.64.le.so: simple.lib.c
	$(CLANG) $(CFLAGS) \
		-shared -fPIC -o $@ $< --target=aarch64-pc-linux-gnu
	file $@

# macho.64.le.dylib: Mach-O 64-bit dynamically linked shared library arm64
macho.64.le.dylib: simple.lib.c
	$(CLANG) $(CFLAGS) \
		-shared -fPIC -o $@ $< --target=aarch64-apple-darwin
	file $@

# I couldn't figure out how to build some "old"-style binaries.
# Fortunately there is a repo which has collected binary specimens
# for this very purpose -- thanks @JonathanSalwan !
BIN_SAMPLES_REPO = "https://github.com/JonathanSalwan/binary-samples/raw/refs/heads/master"

# macho.32.le.exe: Mach-O executable i386
macho.32.le.exe: simple.exe.c
	wget -O $@ $(BIN_SAMPLES_REPO)/MachO-OSX-x86-ls
	file $@

# macho.fat.be.exe (for architecture armv7):	Mach-O executable arm_v7
# macho.fat.be.exe (for architecture armv7s):	Mach-O executable arm_v7s
# macho.fat.be.exe (for architecture arm64):	Mach-O 64-bit executable arm64
macho.fat.be.exe: simple.exe.c
	wget -q -O $@ $(BIN_SAMPLES_REPO)/MachO-iOS-armv7-armv7s-arm64-Helloworld
	file $@

# macho.fat.le.dylib (for architecture x86_64):	Mach-O 64-bit dynamically linked shared library x86_64
# macho.fat.le.dylib (for architecture i386):	Mach-O dynamically linked shared library i386
macho.fat.le.dylib: simple.exe.c
	wget -q -O $@ $(BIN_SAMPLES_REPO)/libSystem.B.dylib
	file $@

clean:
	rm -rf *exe *.so *.dylib *.dwo *.dwp *.dSYM/
