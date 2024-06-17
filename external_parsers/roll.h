#ifndef ROLL_H
#define ROLL_H
#include <stdint.h>

uint32_t rolllength = 1462;
uint8_t roll[] = {61, 51, 0, 3, 77, 33, 0, 3, 51, 22, 0, 2, 77, 33, 0, 3, 87, 33, 0, 3, 103, 5, 0, 0, 92, 5, 0, 0, 87, 5, 0, 0, 69, 39, 0, 3, 77, 33, 0, 3, 51, 81, 0, 15, 103, 5, 0, 0, 92, 5, 0, 0, 87, 5, 0, 0, 69, 39, 0, 3, 77, 33, 0, 3, 51, 22, 0, 2, 77, 33, 0, 3, 87, 33, 0, 3, 103, 5, 0, 0, 92, 5, 0, 0, 87, 5, 0, 0, 69, 39, 0, 3, 77, 33, 0, 3, 51, 83, 0, 0, 69, 4, 0, 2, 69, 4, 0, 2, 69, 4, 0, 2, 69, 4, 0, 8, 69, 4, 0, 14, 34, 5, 0, 6, 34, 11, 0, 1, 29, 11, 0, 1, 34, 11, 0, 1, 38, 16, 0, 31, 32, 11, 0, 1, 29, 11, 0, 1, 32, 16, 0, 1, 29, 5, 0, 0, 25, 16, 0, 31, 29, 5, 0, 6, 29, 11, 0, 1, 32, 11, 0, 1, 34, 11, 0, 1, 29, 11, 0, 13, 25, 11, 0, 1, 51, 16, 0, 7, 51, 11, 0, 1, 38, 34, 0, 38, 29, 5, 0, 6, 29, 11, 0, 1, 32, 11, 0, 1, 34, 11, 0, 1, 29, 11, 0, 1, 34, 11, 0, 1, 38, 11, 0, 13, 32, 11, 0, 1, 29, 11, 0, 1, 32, 16, 0, 1, 29, 5, 0, 0, 25, 16, 0, 31, 29, 5, 0, 6, 29, 11, 0, 1, 32, 11, 0, 1, 34, 11, 0, 1, 29, 11, 0, 1, 25, 11, 0, 13, 38, 5, 0, 6, 38, 5, 0, 6, 38, 11, 0, 1, 43, 11, 0, 1, 38, 40, 0, 8, 34, 58, 0, 2, 38, 11, 0, 1, 43, 11, 0, 1, 34, 11, 0, 1, 38, 5, 0, 6, 38, 5, 0, 6, 38, 11, 0, 1, 43, 11, 0, 1, 38, 11, 0, 1, 25, 11, 0, 1, 25, 22, 0, 50, 29, 11, 0, 1, 32, 11, 0, 1, 34, 11, 0, 1, 29, 11, 0, 13, 38, 11, 0, 1, 43, 11, 0, 1, 38, 28, 0, 8, 25, 5, 0, 0, 29, 5, 0, 0, 34, 5, 0, 0, 29, 5, 0, 0, 43, 1, 69, 1, 43, 1, 43, 1, 69, 1, 43, 1, 43, 1, 69, 1, 43, 1, 43, 1, 69, 1, 69, 1, 69, 1, 69, 1, 69, 1, 69, 1, 69, 1, 43, 1, 43, 1, 69, 1, 43, 1, 43, 1, 43, 1, 43, 0, 0, 1, 38, 1, 77, 1, 38, 1, 38, 1, 77, 1, 38, 1, 38, 1, 77, 1, 38, 1, 38, 1, 77, 1, 38, 1, 38, 1, 77, 1, 38, 1, 38, 1, 77, 1, 38, 1, 38, 1, 77, 1, 38, 1, 38, 1, 38, 1, 0, 2, 25, 1, 51, 1, 25, 1, 25, 1, 51, 0, 51, 1, 29, 1, 29, 1, 51, 1, 34, 1, 34, 1, 51, 1, 34, 1, 51, 0, 29, 1, 51, 1, 29, 1, 29, 1, 0, 0, 77, 1, 38, 1, 38, 1, 77, 1, 38, 1, 38, 1, 77, 1, 38, 1, 38, 1, 77, 1, 38, 1, 77, 1, 77, 1, 77, 1, 77, 1, 77, 1, 38, 1, 38, 1, 77, 1, 38, 1, 38, 1, 77, 1, 38, 1, 38, 0, 0, 1, 87, 1, 34, 1, 34, 1, 87, 1, 34, 1, 34, 1, 87, 1, 34, 1, 34, 1, 87, 1, 34, 1, 87, 1, 87, 1, 32, 1, 32, 1, 87, 1, 29, 1, 29, 1, 87, 1, 29, 1, 29, 1, 87, 1, 29, 1, 29, 0, 0, 1, 103, 1, 25, 1, 25, 1, 103, 1, 0, 0, 29, 1, 29, 1, 92, 1, 29, 1, 0, 0, 34, 1, 87, 1, 34, 1, 34, 1, 0, 0, 69, 1, 29, 1, 29, 1, 69, 1, 34, 1, 34, 1, 69, 1, 34, 1, 34, 1, 69, 1, 34, 1, 34, 1, 69, 1, 34, 1, 34, 1, 69, 1, 34, 1, 34, 1, 69, 1, 69, 1, 38, 1, 69, 1, 38, 1, 38, 1, 69, 1, 38, 1, 38, 1, 38, 0, 0, 1, 32, 1, 32, 1, 77, 1, 32, 1, 32, 1, 77, 1, 32, 1, 32, 1, 77, 1, 32, 1, 32, 1, 77, 1, 29, 1, 29, 1, 77, 1, 29, 1, 77, 0, 25, 1, 77, 1, 25, 1, 25, 1, 77, 1, 25, 1, 25, 1, 25, 0, 0, 1, 25, 1, 25, 1, 51, 1, 25, 1, 25, 1, 51, 1, 25, 1, 25, 0, 51, 1, 51, 1, 38, 1, 38, 1, 51, 1, 38, 1, 38, 1, 51, 1, 38, 1, 38, 1, 51, 1, 38, 1, 38, 1, 51, 1, 38, 1, 38, 1, 51, 1, 38, 1, 38, 1, 51, 1, 38, 1, 38, 1, 51, 1, 38, 1, 51, 0, 51, 1, 51, 1, 34, 1, 34, 1, 51, 1, 34, 1, 34, 1, 51, 1, 34, 1, 34, 1, 51, 1, 34, 1, 34, 1, 51, 1, 34, 1, 34, 1, 51, 1, 34, 1, 34, 1, 51, 1, 34, 1, 34, 1, 51, 1, 34, 1, 34, 1, 34, 1, 34, 1, 34, 1, 34, 1, 34, 1, 34, 1, 34, 1, 0, 2, 25, 1, 103, 1, 25, 1, 25, 1, 0, 0, 92, 1, 29, 1, 29, 1, 92, 1, 0, 0, 34, 1, 34, 1, 87, 1, 34, 1, 0, 0, 29, 1, 69, 1, 29, 1, 29, 1, 69, 0, 69, 1, 43, 1, 43, 1, 69, 1, 43, 1, 43, 1, 69, 1, 43, 1, 43, 1, 69, 1, 43, 1, 69, 1, 69, 1, 69, 1, 69, 1, 69, 1, 43, 1, 43, 1, 69, 1, 43, 1, 43, 1, 69, 1, 43, 1, 43, 0, 0, 1, 77, 1, 38, 1, 38, 1, 77, 1, 38, 1, 38, 1, 77, 1, 38, 1, 38, 1, 77, 1, 38, 1, 38, 1, 77, 1, 38, 1, 38, 1, 77, 1, 38, 1, 38, 1, 77, 1, 38, 1, 38, 1, 77, 1, 38, 1, 0, 2, 51, 1, 25, 1, 25, 1, 51, 1, 29, 1, 29, 1, 51, 1, 29, 1, 51, 0, 34, 1, 51, 1, 34, 1, 34, 1, 51, 0, 51, 1, 29, 1, 29, 1, 29, 1, 0, 0, 51, 1, 51, 1, 77, 1, 51, 1, 51, 1, 77, 1, 51, 1, 51, 1, 77, 1, 51, 1, 51, 1, 77, 1, 51, 1, 51, 1, 77, 1, 77, 1, 32, 1, 77, 1, 32, 1, 32, 1, 77, 1, 32, 1, 32, 1, 32, 0, 0, 1, 34, 1, 34, 1, 87, 1, 34, 1, 34, 1, 87, 1, 34, 1, 34, 1, 87, 1, 34, 1, 34, 1, 87, 1, 32, 1, 32, 1, 87, 1, 32, 1, 87, 0, 29, 1, 87, 1, 29, 1, 29, 1, 87, 1, 29, 1, 29, 1, 29, 0, 0, 1, 25, 1, 25, 1, 103, 1, 25, 1, 0, 0, 29, 1, 92, 1, 29, 1, 29, 1, 0, 0, 87, 1, 34, 1, 34, 1, 87, 1, 0, 0, 29, 1, 29, 1, 69, 1, 29, 1, 69, 0, 34, 1, 69, 1, 34, 1, 34, 1, 69, 1, 34, 1, 34, 1, 69, 1, 34, 1, 34, 1, 69, 1, 34, 1, 34, 1, 69, 1, 34, 1, 69, 0, 69, 1, 69, 1, 38, 1, 38, 1, 69, 1, 38, 1, 38, 1, 38, 1, 38, 0, 0, 1, 32, 1, 77, 1, 32, 1, 32, 1, 77, 1, 32, 1, 32, 1, 77, 1, 32, 1, 32, 1, 77, 1, 77, 1, 29, 1, 77, 1, 29, 1, 29, 1, 77, 0, 77, 1, 25, 1, 25, 1, 77, 1, 25, 1, 25, 1, 25, 1, 25, 1, 25, 1, 51, 1, 25, 1, 51, 1, 51, 1, 51, 1, 51, 1, 51, 1, 25, 1, 25, 1, 51, 1, 25, 1, 25, 1, 51, 1, 25, 1, 25, 0, 51, 1, 51, 1, 38, 1, 38, 1, 51, 1, 38, 1, 38, 1, 51, 1, 38, 1, 38, 1, 51, 1, 38, 1, 38, 1, 51, 1, 38, 1, 38, 1, 51, 0, 51, 1, 34, 1, 34, 1, 51, 1, 34, 1, 34, 1, 51, 1, 34, 1, 34, 1, 51, 1, 34, 1, 34, 1, 51, 1, 34, 1, 34, 1, 51, 1, 34, 1, 34, 1, 51, 1, 34, 1, 34, 1, 51, 1, 34, 1, 34, 1, 51, 1, 34, 0, 34, 1, 34, 1, 69, 1, 34, 0, 34, 1, 34, 1, 69, 1, 34, 1, 34, 1, 69, 1, 34, 1, 34, 1, 0, 2, 69, 1, 69, 1, 69, 1, 0, 8, 69, 1, 69, 1, 69, 1};

#endif