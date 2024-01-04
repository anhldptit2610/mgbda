#include "mmu.h"

void mmu_write(struct gb *gb, uint16_t addr, uint8_t val)
{
    gb->mem[addr] = val;
}

uint8_t mmu_read(struct gb *gb, uint16_t addr)
{
    return gb->mem[addr];
}