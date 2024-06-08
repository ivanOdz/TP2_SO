// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <libc.h>
#include <stdint.h>

void testExceptions(int argc, char **argv) {
	int8_t ex = stringToInt(argv[1], strlen(argv[1]));
	switch (ex) {
		case 1:
			exit(argc / argv[1][strlen(argv[1])]);
			break;
		case 2:
			invalidOpcode();
			break;
		default:
			fprintf(STD_ERR, "Invalid error code\n");
	}
	exit(0);
}

void testArgv(int argc, char **argv) {
	printf("received %d arguments:\n", argc);
	for (uint32_t i = 0; i < argc; i++) {
		printf("argument %d = \'%s\'\n", i, argv[i]);
	}
	exit(0);
}