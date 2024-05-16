include ../Makefile.inc

SHELL_CODE=shellCodeModule.bin

all: shellCodeModule

shellCodeModule:
	cd Shell; make

clean:
	cd Shell; make clean
	rm -rf *.bin *.elf
	


.PHONY: shellCodeModule all clean