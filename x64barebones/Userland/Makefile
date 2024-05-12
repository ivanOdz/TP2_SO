include ../Makefile.inc

SHELL_CODE=shellCodeModule.bin

all: shellCodeModule

shellCodeModule:
	cd ShellCodeModule; make

clean:
	cd ShellCodeModule; make clean
	rm -rf *.bin *.elf
	


.PHONY: shellCodeModule all clean