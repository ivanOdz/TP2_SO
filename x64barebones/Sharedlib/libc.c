// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <libc.h>
#include <stdarg.h>
static uint32_t rand_seed = 1;

uint64_t pow(uint64_t base, uint64_t exp) {
	uint64_t ans = 1;
	while (exp--) {
		ans *= base;
	}
	return ans;
}

int64_t strcpy(char *dest, const char *src) {
	int64_t i = 0;
	while (*src != 0) {
		*dest++ = *src++;
		i++;
	}
	*dest = 0;
	return i;
}

int64_t strcmp(const char *str1, const char *str2) {
	while (*str1 != '\0' || *str2 != '\0') {
		if (*str1 < *str2) {
			return -1;
		} // str1 < str2
		else if (*str1 > *str2) {
			return 1;
		} // str1 > str2

		str1++;
		str2++;
	}

	return 0; // str1 equals str2 (same content)
}

int64_t strincludes(const char *contains, const char *contained) {
	while (*contained != '\0') {
		if (*(contains++) != *(contained++))
			return FALSE;
	}
	return TRUE;
}

int64_t stringToInt(char *str, uint32_t strlen) {
	int64_t ans = 0;
	int8_t sign = 1;
	if (str[0] == '-') {
		sign = -1;
		str++;
		strlen--;
	}
	for (int i = 0; strlen != i; i++) {
		if (str[i] > '9' || str[i] < '0')
			return 0;
		ans += pow(10, (strlen - i - 1)) * (str[i] - '0');
	}
	return ans * sign;
}

uint64_t strlen(const char *str) {
	uint64_t cont;

	for (cont = 0; str[cont] != '\0'; cont++) {
		continue;
	}

	return cont;
}

static uint32_t uintToBase(uint64_t value, char *buffer, uint32_t base) {
	char *p = buffer;
	char *p1, *p2;
	uint32_t digits = 0;

	// Calculate characters for each digit
	do {
		uint32_t remainder = value % base;
		*p++ = (remainder < 10) ? remainder + '0' : remainder + 'A' - 10;
		digits++;

	} while (value /= base);

	// Terminate string in buffer
	*p = 0;

	// Reverse string in buffer
	p1 = buffer;
	p2 = p - 1;

	while (p1 < p2) {
		char tmp = *p1;
		*p1 = *p2;
		*p2 = tmp;
		p1++;
		p2--;
	}
	return digits;
}

// printf solo puede lidiar con enteros unsigned, char, string y hexa (numeros solo en uint64_t)
void printf(char *fmt, ...) {
	va_list args;
	va_start(args, fmt);
	fprintf_args(STD_OUT, fmt, args);
	va_end(args);
}

void fprintf(uint8_t fd, char *fmt, ...) {
	va_list args;
	va_start(args, fmt);
	fprintf_args(fd, fmt, args);
	va_end(args);
}

void fprintf_args(uint8_t fd, char *fmt, va_list args) {
	char buffer[0xFF] = {0};

	uint64_t i, j;
	uint64_t padding;
	for (i = 0, j = 0; fmt[i] != 0; i++) {
		if (fmt[i] == '%') {
			SyscallWrite(fd, (fmt + j), i - j);
			i++;
			uint8_t padRight = 0;
			if (fmt[i] == '-') {
				i++;
				padRight++;
			}
			j = i;

			while (fmt[i] >= '0' && fmt[i] <= '9')
				i++;
			padding = stringToInt((fmt + j), i - j);
			if (padding >= 100)
				padding = 99;
			if (fmt[i] == 'l') {
				i++;
			}
			switch (fmt[i]) {
				case 'u':
				case 'd':
				case 'i':
					uintToBase(va_arg(args, uint64_t), buffer, 10);
					printPadded(fd, buffer, '0', padding, padRight);
					break;
				case 'c':
				case '%':
					fputchar(fd, va_arg(args, int));
					break;
				case 's':
					printPadded(fd, va_arg(args, char *), ' ', padding, padRight);
					break;
				case 'x':
				case 'X':
					uintToBase(va_arg(args, uint64_t), buffer, 16);
					printPadded(fd, buffer, '0', padding, padRight);
				case 'l':
					break; // we ignore this because we always print as uint64_t but PVS wants the l
			}

			j = ++i;
		}
	}
	SyscallWrite(fd, (fmt + j), i - j);
	return;
}

uint64_t argumentParse(int arg, int argc, char **argv) {
	if (arg + 1 >= argc) {
		fprintf(STD_ERR, "Argument error, missing value\n");
		return 0;
	}
	else {
		uint64_t result = stringToInt(argv[arg + 1], strlen(argv[arg + 1]));
		if (!result)
			fprintf(STD_ERR, "Argument error, expected value for %s, got %s\n", argv[arg], argv[arg + 1]);
		return result;
	}
}

void printPadded(uint8_t fd, char *buffer, uint8_t pad, uint64_t totalLen, uint8_t padRight) {
	uint32_t bufferLen = strlen(buffer);
	if (!totalLen && bufferLen)
		totalLen = bufferLen;
	if (padRight) {
		SyscallWrite(fd, buffer, (bufferLen < totalLen) ? bufferLen : totalLen);
		for (uint64_t i = strlen(buffer); i < totalLen; i++) {
			fputchar(fd, pad);
		}
	}
	else {
		for (uint64_t i = strlen(buffer); i < totalLen; i++) {
			fputchar(fd, pad);
		}
		SyscallWrite(fd, buffer, (bufferLen < totalLen) ? bufferLen : totalLen);
	}
	return;
}

void putchar(char c) {
	fputchar(STD_OUT, c);
}

void fputchar(uint8_t fd, char c) {
	SyscallWrite(fd, &c, 1);
}

void fputs(uint8_t fd, char *str) {
	SyscallWrite(fd, str, strlen(str));
}

void puts(char *str) {
	fputs(STD_OUT, str);
}

void text_color_set(uint32_t color) {
	text_format format;

	SyscallGetFormat(&format);

	if (format.fg != color) {
		format.fg = color;
		SyscallSetFormat(&format);
	}
}

uint32_t text_color_get() {
	text_format format;

	SyscallGetFormat(&format);

	return format.fg;
}

void srand(uint32_t seed) {
	rand_seed = seed;
}

uint32_t rand() {
	uint32_t new_rand = (rand_seed * 31 * 29 * 23 * 19 * 17 / 13 / 11 / 7 / 5 / 3) & 0xFFFFFFFF;
	srand(new_rand);
	return new_rand;
}

uint32_t randBetween(uint32_t start, uint32_t end) {
	return rand() % (end - start) + start;
}

void memoryManagerStats(MemoryInfo *meminfo) {
	SyscallMemInfo(meminfo);
}