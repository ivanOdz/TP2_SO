include ../../Makefile.inc

MODULE=../shellCodeModule.bin
SOURCES=$(wildcard *.c)
SOURCES_ASM=$(wildcard asm/*.asm)
OBJECTS=$(SOURCES:.c=.o)
OBJECTS_ASM=$(SOURCES_ASM:.asm=.o)
LOADERSRC=_loader.c

STATICLIBS=

all: $(MODULE)

$(MODULE): $(LOADEROBJECT) $(OBJECTS) $(STATICLIBS) $(OBJECTS_ASM)
	@echo "LINKING SHELL BIN"
	@$(LD) $(LDFLAGS) -T shellCodeModule.ld -o $(MODULE) $(LOADEROBJECT) $(OBJECTS) $(OBJECTS_ASM) $(STATICLIBS)
	@echo "LINKING SHELL ELF"
	@$(LD) $(LDFLAGS) -T shellCodeModule.ld --oformat=elf64-x86-64 -o ../shellCodeModule.elf $(LOADEROBJECT) $(OBJECTS) $(OBJECTS_ASM) $(STATICLIBS)

%.o: %.c
	@echo "COMPILING $<"
	@$(GCC) $(GCCFLAGS) -I./include -c $< -o $@

%.o : %.asm
	@echo "COMPILING $<"
	@$(ASM) $(ASMFLAGS) $< -o $@

$(LOADEROBJECT):
	@echo "COMPILING $(LOADEROBJECT)"
	@$(GCC) $(GCCFLAGS) $(LOADERSRC) -o $(LOADEROBJECT)

clean:
	rm -rf asm/*.o *.o *.bin

.PHONY: all clean
