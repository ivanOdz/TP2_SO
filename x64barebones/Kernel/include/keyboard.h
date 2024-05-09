#ifndef _KEYBOARD_H_
#define _KEYBOARD_H_
#include <stdint.h>
void keyboard_handler();
uint64_t consume_keys(uint8_t* buf, uint64_t size);


#define STD_IN 0

#define TECLASVALIDAS 128

static const char toAscii[128] = {0, '\e', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', 8, 9, 
                    'q','w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[',']', 10, 0, 'a', 's',
                    'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`', 0, '\\', 'z', 'x', 'c',
                    'v', 'b', 'n', 'm', ',','.', '/', 0, '*', 0, ' ', 0};

static const char mods[] = {0, '\e', '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', 8, 9,
                    'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', 10, 0, 'A', 'S',
                    'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '"', '~', 0, '|', 'Z', 'X', 'C', 
                    'V', 'B', 'N', 'M', '<', '>', '?', 0, '*', 0, ' ', 0};
                    
extern uint8_t getKey();
uint8_t consume_key();

#endif
