#pragma once

typedef enum CPU_MODE {
    NORMAL,
    HALT,
    HALT_BUG,
    IME,
    STOP,
} cpu_mode_t;

struct cpu_register {
    uint8_t a;
    uint8_t b;
    uint8_t c;
    uint8_t d;
    uint8_t e;
    uint8_t f;
    uint8_t h;
    uint8_t l;
    uint16_t pc;
    uint16_t sp;
};

struct cpu {
    struct cpu_register regs;
    cpu_mode_t mode;
};

struct rom_info {
    uint32_t size;
    bool loaded;
};

struct rom {
    uint8_t *data;
    struct rom_info info;
};

struct gb {
    uint8_t mem[GB_MEM_SIZE];
    struct cpu cpu;
    struct rom rom;
};

struct gb *gb_create(void);
void gb_destroy(struct gb *gb);