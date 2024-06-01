include ../Makefile.inc

KERNEL=kernel.bin
SOURCES=$(wildcard *.c)
SOURCES_DRIVERS=$(wildcard Drivers/*.c)
SOURCES_IRQ=$(wildcard Irq/*.c)
SOURCES_ASM=$(wildcard Asm/*.asm)
OBJECTS=$(SOURCES:.c=.o)
OBJECTS_ASM=$(SOURCES_ASM:.asm=.o)
OBJECTS_IRQ=$(SOURCES_IRQ:.c=.o)
OBJECTS_DRIVERS=$(SOURCES_DRIVERS:.c=.o)
SOURCES_MM=$(wildcard memoryManagers/*.c)
OBJECTS_MM=$(SOURCES_MM:.c=.o)
SOURCES_PROCESSES=$(wildcard processes/*.c)
OBJECTS_PROCESSES=$(SOURCES_PROCESSES:.c=.o)
LOADERSRC=./loader.asm

LOADEROBJECT=$(LOADERSRC:.asm=.o)
STATICLIBS=../Sharedlib/libc.o
all: $(KERNEL)

$(KERNEL): $(LOADEROBJECT) $(OBJECTS_DRIVERS) $(OBJECTS_IRQ) $(OBJECTS_MM) $(OBJECTS_PROCESSES) $(OBJECTS) $(STATICLIBS) $(OBJECTS_ASM)
	@echo "LINKING KERNEL BIN"
	@$(LD) $(LDFLAGS) -T kernel.ld -o $(KERNEL) $(LOADEROBJECT) $(OBJECTS) $(OBJECTS_ASM) $(STATICLIBS) $(OBJECTS_DRIVERS) $(OBJECTS_IRQ) $(OBJECTS_MM) $(OBJECTS_PROCESSES)
	@echo "LINKING KERNEL ELF"
	@$(LD) $(LDFLAGS) -T kernel.ld --oformat=elf64-x86-64 -o kernel.elf $(LOADEROBJECT) $(OBJECTS) $(OBJECTS_ASM) $(STATICLIBS) $(OBJECTS_DRIVERS) $(OBJECTS_IRQ) $(OBJECTS_MM)

%.o: %.c
	@echo "COMPILING $<"
	@$(GCC) $(GCCFLAGS) -I./Include -I./Drivers/Include -I./Irq/Include -I./memoryManagers -I../Sharedlib -c $< -o $@

%.o : %.asm
	@echo "COMPILING $<"
	@$(ASM) $(ASMFLAGS) $< -o $@

$(LOADEROBJECT):
	@echo "COMPILING $(LOADEROBJECT)"
	@$(ASM) $(ASMFLAGS) $(LOADERSRC) -o $(LOADEROBJECT)

clean:
	rm -rf ./**/*.o  *.bin *.elf

.PHONY: all clean
