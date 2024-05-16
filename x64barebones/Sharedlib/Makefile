include ../Makefile.inc

SHELL_CODE=shellCodeModule.bin

SOURCES_C=$(wildcard ./*.c)
SOURCES_ASM=$(wildcard ./*.asm)
OBJECTS_CL=$(SOURCES_C:.c=.o)
OBJECTS_ASM=$(SOURCES_ASM:.asm=.o)

all: lib

lib:
	@echo "COMPILING SHARED LIB"
	@$(GCC) $(GCCFLAGS) -I./ -c libc.c -o libc_c.o
	@$(ASM) $(ASMFLAGS) libc.asm -o libc_asm.o
	@$(LD) -r libc_c.o libc_asm.o -o libc.o

clean:
	rm -rf *.o 

.PHONY: all clean
