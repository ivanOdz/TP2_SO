include Makefile.inc
all:  printflags toolchain bootloader kernel userland image

printflags:
	@echo "\n\nCOMPILING"
	@echo "gccflags:"
	@echo $(GCCFLAGS)
	@echo "ldflags:"
	@echo $(LDFLAGS)
	@echo "\n\n"
toolchain:
	cd Toolchain; make all

bootloader:
	cd Bootloader; make all

kernel:
	cd Kernel; make all

userland:
	cd Userland; make all

image: kernel bootloader userland
	cd Image; make all

clean:
	cd Toolchain; make clean
	cd Bootloader; make clean
	cd Image; make clean
	cd Kernel; make clean
	cd Userland; make clean

.PHONY: toolchain bootloader image collections kernel userland all clean
