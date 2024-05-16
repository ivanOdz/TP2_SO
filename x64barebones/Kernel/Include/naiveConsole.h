#ifndef NAIVE_CONSOLE_H
#define NAIVE_CONSOLE_H

#include <stdint.h>
#include <defs.h>

void ncPrint(const char * string);
void ncPrintChar(char character);
void ncScroll();
void ncBlankLine();
void ncNewline();
void ncPrintDec(uint64_t value);
void ncPrintHex(uint64_t value);
void ncPrintBin(uint64_t value);
void ncPrintBase(uint64_t value, uint32_t base);
void ncClear();
void ncPrintCustomizedMessage(const char * string, uint8_t fmt);
void ncPrintCustomizedChar(char character, uint8_t fmt);
void ncTime();
uint32_t uintToBase(uint64_t value, char * buffer, uint32_t base);
void ncPrintBase2(uint64_t value, uint32_t base);

#endif