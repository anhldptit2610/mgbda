#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#define GB_MEM_SIZE         0x10000
#define CPU_FREQ            4194304
#define SCREEN_WIDTH        160
#define SCREEN_HEIGHT       144

#define TO_U16(lsb, msb) (((uint16_t)(msb) << 8) | (uint16_t)(lsb))
#define MSB(nn)          (((nn) >> 8) & 0xff)
#define LSB(nn)          ((nn) & 0xff)