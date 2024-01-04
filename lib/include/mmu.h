#pragma once

#include "common.h"
#include "gb.h"

void mmu_write(struct gb *gb, uint16_t addr, uint8_t val);
uint8_t mmu_read(struct gb *gb, uint16_t addr);