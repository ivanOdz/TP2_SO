include ../Makefile.inc

SHELL_CODE=shellCodeModule.bin

SOURCES_SHELL=$(wildcard ./Shell/*.c)
SOURCES_ASM=$(wildcard ./Shell/Asm/*.asm)
OBJECTS_SHELL=$(SOURCES_SHELL:.c=.o)
OBJECTS_ASM=$(SOURCES_ASM:.asm=.o)
SOURCES_SNAKE=$(wildcard ./Games/*.c)
OBJECTS_SNAKE=$(SOURCES_SNAKE:.c=.o)
LOADERSRC=./_loader.c

STATICLIBS=../Sharedlib/libc.o

all: loader $(SHELL_CODE)

$(SHELL_CODE): $(LOADEROBJECT) $(OBJECTS_SHELL) $(OBJECTS_SNAKE) $(STATICLIBS) $(OBJECTS_ASM)
	@echo "LINKING USERLAND BIN"
	@$(LD) $(LDFLAGS) -T shellCodeModule.ld -o $(SHELL_CODE) loader.o $(OBJECTS_SHELL) $(OBJECTS_SNAKE) $(OBJECTS_ASM) $(STATICLIBS)
	@echo "LINKING USERLAND ELF"
	@$(LD) $(LDFLAGS) -T shellCodeModule.ld --oformat=elf64-x86-64 -o shellCodeModule.elf loader.o $(OBJECTS_SHELL) $(OBJECTS_SNAKE) $(OBJECTS_ASM) $(STATICLIBS)

%.o: %.c
	@echo "COMPILING $<"
	@$(GCC) $(GCCFLAGS) -I./Shell/Include -I./Games -I./../Sharedlib -c $< -o $@

%.o : %.asm
	@echo "COMPILING $<"
	@$(ASM) $(ASMFLAGS) $< -o $@

loader:
	@echo "COMPILING $(LOADERSRC)"
	@$(GCC) -c $(GCCFLAGS) _loader.c -o loader.o

clean:
	rm -rf Shell/Asm/*.o Shell/*.o Games/*.o *.o *.bin *.elf

.PHONY: all clean

.PHONY: shellCodeModule all clean