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
LOADERSRC=Asm/loader.asm

LOADEROBJECT=$(LOADERSRC:.asm=.o)
STATICLIBS=

all: $(KERNEL)

$(KERNEL): $(LOADEROBJECT) $(OBJECTS_DRIVERS) $(OBJECTS_IRQ) $(OBJECTS) $(STATICLIBS) $(OBJECTS_ASM)
	echo "LINKING KERNEL BIN"
	@$(LD) $(LDFLAGS) -T kernel.ld -o $(KERNEL) $(OBJECTS) $(OBJECTS_ASM) $(STATICLIBS) $(OBJECTS_DRIVERS) $(OBJECTS_IRQ) 
	echo "LINKING KERNEL ELF"
	@$(LD) $(LDFLAGS) -T kernel.ld --oformat=elf64-x86-64 -o kernel.elf $(OBJECTS) $(OBJECTS_ASM) $(STATICLIBS) $(OBJECTS_DRIVERS) $(OBJECTS_IRQ) 

%.o: %.c
	@echo "COMPILING $<"
	@$(GCC) $(GCCFLAGS) -I./Include -I./Drivers/Include -I./Irq/Include -c $< -o $@

%.o : %.asm
	@echo "COMPILING $<"
	@$(ASM) $(ASMFLAGS) $< -o $@

$(LOADEROBJECT):
	@echo "COMPILING $(LOADEROBJECT)"
	@$(ASM) $(ASMFLAGS) $(LOADERSRC) -o $(LOADEROBJECT)

clean:
	rm -rf asm/*.o *.o Drivers/*.o Irq/*.o *.bin *.elf

.PHONY: all clean
