#include <naiveConsole.h>
#include <videoDriver.h>
#include <lib.h>

#define DEFAULT_FMT	0x07
#define ERROR_FMT	0x04

static char buffer[64] = { '0' };
static uint8_t * const video = (uint8_t*)0xB8000;
static uint8_t * currentVideo = (uint8_t*)0xB8000;
static const uint32_t width = 80;
static const uint32_t height = 25;

void ncPrint(const char * string)
{
	int i;
	for (i = 0; string[i] != 0; i++)
		ncPrintChar(string[i]);
}

void ncPrintChar(char character)
{
	if(((uint64_t)(currentVideo - video)) >= width*height*2) ncScroll();
	*currentVideo = character;
	currentVideo += 2;
}

void ncScroll(){
	for (int i = 0; i < height; i++){
		if (i != height-1)
			memcpy(video + i * width * 2, video + (i+1) * width * 2, width * 2);
		else {
			currentVideo -= width * 2;
			ncBlankLine();
		}
	}
	currentVideo -= width * 2;
}

void ncBlankLine(){
	do
	{
		ncPrintCustomizedChar(' ', DEFAULT_FMT);
	}
	while((uint64_t)(currentVideo - video) % (width * 2) != 0);
}

void ncNewline()
{
	ncBlankLine();
	if(((uint64_t)(currentVideo - video)) >= width*height*2) ncScroll();
}

void ncPrintDec(uint64_t value)
{
	ncPrintBase(value, 10);
}

void ncPrintHex(uint64_t value)
{
	ncPrintBase(value, 16);
}

void ncPrintBin(uint64_t value)
{
	ncPrintBase(value, 2);
}

void ncPrintBase(uint64_t value, uint32_t base)
{
    uintToBase(value, (char *)buffer, base);
    ncPrint(buffer);
}
void ncPrintBase2(uint64_t value, uint32_t base)
{
    uintToBase(value, (char *)buffer, base);
    //ncPrint(buffer);
	print((uint8_t *)buffer);
}

void ncClear()
{
	int i;
	currentVideo=video;
	for (i = 0; i < height * width; i++)
		ncPrintCustomizedChar(' ', DEFAULT_FMT);
	currentVideo = video;
}

uint32_t uintToBase(uint64_t value, char * buffer, uint32_t base)
{
	char *p = buffer;
	char *p1, *p2;
	uint32_t digits = 0;

	//Calculate characters for each digit
	do
	{
		uint32_t remainder = value % base;
		*p++ = (remainder < 10) ? remainder + '0' : remainder + 'A' - 10;
		digits++;
	}
	while (value /= base);

	// Terminate string in buffer.
	*p = 0;

	//Reverse string in buffer.
	p1 = buffer;
	p2 = p - 1;
	while (p1 < p2)
	{
		char tmp = *p1;
		*p1 = *p2;
		*p2 = tmp;
		p1++;
		p2--;
	}

	return digits;
}

void ncPrintCustomizedChar(char character, uint8_t fmt)
{
	*currentVideo = character;
	currentVideo++;
	*currentVideo = fmt;
	currentVideo++;
}

void ncPrintCustomizedMessage(const char * string, uint8_t fmt)
{
	int i;
	for (i = 0; string[i] != 0; i++)
		ncPrintCustomizedChar(string[i], fmt);
}
/*
void sys_write(uint8_t fd, uint8_t * buffer, uint64_t size)
{
	int i;
    uint8_t fmt;

	if (fd == STD_FMT) {		// Buffer used to set format

		staticTextColor = *buffer;
	}
	else {						// Buffer used as a String

		if 		(fd == STD_OUT) { fmt = DEFAULT_FMT; }
		else if (fd == STD_ERR) { fmt = ERROR_FMT; }
		else if (fd == STD_CMD) { fmt = staticTextColor; }

		for (i = 0; buffer[i] != 0 && i < size; i++) {

			ncPrintCustomizedChar(buffer[i], fmt);
		}
	}
}
*/