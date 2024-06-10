#ifndef LIBC_H
#define LIBC_H

#include <stdarg.h>
#include <stdint.h>

#define NULL  0
#define TRUE  1
#define FALSE 0
#define EOF	  (char) -1

#define STD_IN	0
#define STD_OUT 1
#define STD_ERR 2

#define HZ 50

#define COLORS_BLUE	  0x000000FF
#define COLORS_GREEN  0x0000FF00
#define COLORS_RED	  0x00FF0000
#define COLORS_BLACK  0x00000000
#define COLORS_WHITE  0x00FFFFFF
#define COLORS_ORANGE 0x00EF3F00
#define COLORS_CIAN	  0x0000E0FF

typedef char bool;

typedef enum { FOREGROUND = 0,
			   BACKGROUND,
			   RELEGATED } ProcessRunMode;
typedef uint16_t PID_t;

typedef struct text_format {
	uint32_t bg;
	uint32_t fg;
	uint8_t size;
	uint8_t enableCursorBlink;
} text_format;

typedef struct {
	uint8_t hora;
	uint8_t min;
	uint8_t seg;
} time_type;

typedef struct {
	uint16_t x, y;
	uint16_t width, height;
	uint8_t border_width;
	uint32_t border_color;
	uint32_t *texture;
} draw_type;

typedef struct MemoryInfo {
	void *startAddress;
	uint64_t totalMemory;
	uint64_t freeMemory;
	uint64_t occupiedMemory;
	uint64_t fragmentedMemory;
	uint64_t minFragmentedSize;
	uint64_t maxFragmentedSize;
	uint64_t assignedNodes;
	void *endAddress;
	uint8_t mmType;
} MemoryInfo;

typedef struct ProcessInfo {
	char name[255];
	PID_t pid;
	PID_t parent_PID;
	char runMode;
	void *stackBasePointer;
	void *stackPointer;
	char *processStatus;
	uint8_t priority;
	struct ProcessInfo *nextProcessInfo;

} ProcessInfo;

typedef struct FdInfo{
	char *processName;
	PID_t pid;
	uint16_t fd;
	uint8_t mode;
	char *pipeName;
	struct FdInfo *nextFdInfo;
} FdInfo;

typedef struct ReturnStatus {
	int returnValue;
	uint8_t aborted;
	PID_t pid;
} ReturnStatus;

extern uint64_t SyscallWrite(uint8_t fd, char *buffer, uint64_t size);
extern uint64_t SyscallRead(uint8_t fd, char *buffer, uint64_t size);
extern uint64_t SyscallClear();
extern uint64_t SyscallGetRegisters();
extern uint64_t SyscallGetRTC(time_type *buffer);
extern uint64_t SyscallGetFormat(text_format *buffer);
extern uint64_t SyscallSetFormat(text_format *buffer);
extern uint64_t SyscallPutBlock(draw_type *draw);
extern uint64_t SyscallGetTicks();
extern uint64_t SyscallAudio(uint8_t isLoop, const uint8_t *buffer, uint64_t size);
extern uint64_t SyscallSetTimer(uint16_t delay);
extern uint64_t SyscallMemInfo(MemoryInfo *meminfo);
extern uint64_t SyscallPrintMem();
extern ProcessInfo *SyscallProcessInfo();
extern uint64_t SyscallNice(uint16_t pid, uint8_t newPriority);
extern uint64_t SyscallToggleBlockProcess(uint16_t pid);
extern FdInfo *SyscallFdInfo(uint16_t pid);


int64_t strcpy(char *dest, const char *src);
int64_t strcmp(const char *str1, const char *str2);
int64_t strincludes(const char *contains, const char *contained);
uint64_t strlen(const char *str);
int64_t stringToInt(char *str, uint32_t strlen);
uint32_t uintToBase(uint64_t value, char *buffer, uint32_t base);
uint32_t rand();
void srand(uint32_t seed);
uint32_t randBetween(uint32_t start, uint32_t end);

void puts(char *str);
void putchar(char c);
void fputchar(uint8_t fd, char c);
void fputs(uint8_t fd, char *str);
char getchar();
void printf(char *fmt, ...);
void fprintf(uint8_t fd, char *fmt, ...);
void fprintf_args(uint8_t fd, char *fmt, va_list args);
uint64_t argumentParse(int arg, int argc, char **argv);
void text_color_set(uint32_t color);
uint32_t text_color_get();
int command_argvTest(int argc, char **argv);
void printPadded(uint8_t fd, char *buffer, uint8_t pad, uint64_t totalLen, uint8_t padRight);
extern void invalidOpcode();
void *malloc(uint64_t size);
void free(void *memory);
void memoryManagerStats(MemoryInfo *meminfo);
PID_t execv(void (*processMain)(int argc, char **argv), char **argv, ProcessRunMode runMode);
void exit(int returnValue);
PID_t waitpid(PID_t PID, ReturnStatus *wstatus);
void yield();
PID_t getPID();
PID_t kill(PID_t PID);
void sleep(uint64_t ms);
int64_t pipe(char *name, int pipefd[2]);

#endif