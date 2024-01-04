#pragma once

#include "common.h"
#include "gb.h"
#include "mmu.h"

typedef enum R8 {
    R8_B,
    R8_C,
    R8_D,
    R8_E,
    R8_F,
    R8_H,
    R8_L,
    R8_A,
} cpu_r8_t;

typedef enum R16 {
    R16_BC,
    R16_DE,
    R16_HL,
    R16_SP,
    R16_AF,
    R16_PC,
} cpu_r16_t;

typedef enum CPU_COND {
    COND_NZ,
    COND_Z,
    COND_NC,
    COND_C,
} cpu_cond_t;

typedef enum CPU_FLAGS {
    FLAG_C = (1U << 4),
    FLAG_H = (1U << 5),
    FLAG_N = (1U << 6),
    FLAG_Z = (1U << 7),
} cpu_flag_t;

typedef enum CPU_OPERATOR {
    ADD,
    SUB,
    AND,
    OR,
    XOR,
} cpu_operator_t;

void cpu_step(struct gb *gb);
void tick(struct gb *gb);
void cpu_cycle(struct gb *gb);
void cpu_init(struct gb *gb);