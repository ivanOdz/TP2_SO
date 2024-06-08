include Makefile.inc
all:  printflags toolchain bootloader sharedlib kernel userland image

printflags:
	@echo "\n\nCOMPILING"
	@echo "gccflags:"
	@echo $(GCCFLAGS)
	@echo "ldflags:"
	@echo $(LDFLAGS)
	@echo "\n\n"

pvs: clean toolchain bootloader pvs_test

pvs_test:
	@tput setaf 4
	@echo -e "\n\nRUNNING PVS-STUDIO TEST\n\n"
	@tput sgr0
	@pvs-studio-analyzer trace -- make sharedlib kernel userland
	@pvs-studio-analyzer analyze
	@rm -f strace_out
	@plog-converter -a '64:1,2,3;GA:1,2,3;OP:1,2,3' -t tasklist -o report.tasks PVS-Studio.log
	@rm -f PVS-Studio.log

	@if [ -s "report.tasks" ]; then \
		tput setaf 1; \
		echo -e "\n\nPVS-STUDIO ENCOUNTERED AN ERROR\n"; \
		cat report.tasks; \
	else \
		tput setaf 2; \
		echo -e "\n\nPVS-STUDIO TEST FINISHED SUCCESSFULLY\n"; \
	fi
	@tput sgr0
	
toolchain:
	cd Toolchain; make all

bootloader:
	cd Bootloader; make all

sharedlib:
	cd Sharedlib; make all

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
	cd Sharedlib; make clean
	cd Kernel; make clean
	cd Userland; make clean

.PHONY: sharedlib toolchain bootloader image collections kernel userland all clean
