#include "cpu.h"

static uint16_t get_r16(struct gb *gb, cpu_r16_t rr)
{
    uint16_t ret;

    switch (rr) {
    case R16_BC:
        ret = TO_U16(gb->cpu.regs.c, gb->cpu.regs.b);
        break;
    case R16_DE:
        ret = TO_U16(gb->cpu.regs.e, gb->cpu.regs.d);
        break;
    case R16_HL:
        ret = TO_U16(gb->cpu.regs.l, gb->cpu.regs.h);
        break;
    case R16_AF:
        ret = TO_U16(gb->cpu.regs.f, gb->cpu.regs.a);
        break;
    case R16_SP:
        ret = gb->cpu.regs.sp;
        break;
    case R16_PC:
        ret = gb->cpu.regs.pc;
        break;
    default:
        ret = 0xffff;
        break;
    }
    return ret;
}

static void set_r16(struct gb *gb, cpu_r16_t rr, uint16_t val)
{
    switch (rr) {
    case R16_BC:
        gb->cpu.regs.b = MSB(val);
        gb->cpu.regs.c = LSB(val);
        break;
    case R16_DE:
        gb->cpu.regs.d = MSB(val);
        gb->cpu.regs.e = LSB(val);
        break;
    case R16_HL:
        gb->cpu.regs.h = MSB(val);
        gb->cpu.regs.l = LSB(val);
        break;
    case R16_AF:
        gb->cpu.regs.a = MSB(val);
        gb->cpu.regs.f = LSB(val) & 0xf0;
        break;
    case R16_SP:
        gb->cpu.regs.sp = val;
        break;
    case R16_PC:
        gb->cpu.regs.pc = val;
        break;
    default:
        break;
    }
}

static uint8_t get_r8(struct gb *gb, cpu_r8_t r)
{
    switch (r) {
    case R8_A:
        return gb->cpu.regs.a;
    case R8_B:
        return gb->cpu.regs.b;
    case R8_C:
        return gb->cpu.regs.c;
    case R8_D:
        return gb->cpu.regs.d;
    case R8_E:
        return gb->cpu.regs.e;
    case R8_F:
        return gb->cpu.regs.f;
    case R8_H:
        return gb->cpu.regs.h;
    case R8_L:
        return gb->cpu.regs.l;
    default:
        break;
    }
    return 0xff;
}

static void set_r8(struct gb *gb, cpu_r8_t r, uint8_t val)
{
    switch (r) {
    case R8_A:
        gb->cpu.regs.a = val;
        break;
    case R8_B:
        gb->cpu.regs.b = val;
        break;
    case R8_C:
        gb->cpu.regs.c = val;
        break;
    case R8_D:
        gb->cpu.regs.d = val;
        break;
    case R8_E:
        gb->cpu.regs.e = val;
        break;
    case R8_F:
        gb->cpu.regs.f = val;
        break;
    case R8_H:
        gb->cpu.regs.h = val;
        break;
    case R8_L:
        gb->cpu.regs.l = val;
        break;
    default:
        break;
    }
}

static uint8_t get_flag(struct gb *gb, cpu_flag_t flag)
{
    return (gb->cpu.regs.f & flag) ? 0x01: 0x00;
}

static void set_flag(struct gb *gb, cpu_flag_t flag)
{
    gb->cpu.regs.f |= flag;
}

static void reset_flag(struct gb *gb, cpu_flag_t flag)
{
    gb->cpu.regs.f &= ~flag;
}

static void toggle_flag(struct gb *gb, cpu_flag_t flag, bool val)
{
    if (val)
        set_flag(gb, flag);
    else
        reset_flag(gb, flag);
}

static int check_cond(struct gb *gb, cpu_cond_t cond)
{
    int ret = 0;

    switch (cond) {
    case COND_C:
        if (gb->cpu.regs.f & FLAG_C)
            ret = 1;
        break;
    case COND_NC:
        if (!(gb->cpu.regs.f & FLAG_C))
            ret = 1;
        break;
    case COND_Z:
        if (gb->cpu.regs.f & FLAG_Z)
            ret = 1;
        break;
    case COND_NZ:
        if (!(gb->cpu.regs.f & FLAG_Z))
            ret = 1;
        break;
    default:
        break;
    }
    return ret;
}

// TODO: complete this
void tick(struct gb *gb)
{

}

// TODO: complete this
void cpu_cycle(struct gb *gb)
{
}

static uint8_t cpu_read(struct gb *gb, uint16_t addr)
{
    cpu_cycle(gb);
    return mmu_read(gb, addr);
}

static void cpu_write(struct gb *gb, uint16_t addr, uint8_t val)
{
    cpu_cycle(gb);
    mmu_write(gb, addr, val);
}

void stack_push(struct gb *gb, uint16_t val)
{
    gb->cpu.regs.sp--;
    cpu_write(gb, gb->cpu.regs.sp, MSB(val));
    gb->cpu.regs.sp--;
    cpu_write(gb, gb->cpu.regs.sp, LSB(val));
}

uint16_t stack_pop(struct gb *gb)
{
    uint8_t lsb = cpu_read(gb, gb->cpu.regs.sp);
    gb->cpu.regs.sp++;
    uint8_t msb = cpu_read(gb, gb->cpu.regs.sp);
    gb->cpu.regs.sp++;
    return TO_U16(lsb, msb);
}

/***************************/
/***************************/
/* 8-bit Load Instructions */
/***************************/
/***************************/

static void ld_indirect_nn_a(struct gb *gb)
{
    uint8_t lsb = cpu_read(gb, gb->cpu.regs.pc++);
    uint8_t msb = cpu_read(gb, gb->cpu.regs.pc++); 

    cpu_write(gb, TO_U16(lsb, msb), gb->cpu.regs.a);
}

static void ld_a_indirect_nn(struct gb *gb)
{
    uint8_t lsb = cpu_read(gb, gb->cpu.regs.pc++);
    uint8_t msb = cpu_read(gb, gb->cpu.regs.pc++); 

    gb->cpu.regs.a = cpu_read(gb, TO_U16(lsb, msb));
}

static void ld_r_n(struct gb *gb, cpu_r8_t r)
{
    uint8_t n = cpu_read(gb, gb->cpu.regs.pc++);

    set_r8(gb, r, n);
}

static void ld_indirect_rr_a(struct gb *gb, cpu_r16_t rr)
{
    uint16_t rr_val = get_r16(gb, rr);

    cpu_write(gb, rr_val, gb->cpu.regs.a);
}

static void ldd_indirect_hl_a(struct gb *gb)
{
    uint16_t hl = get_r16(gb, R16_HL);

    cpu_write(gb, hl, get_r8(gb, R8_A));
    set_r16(gb, R16_HL, hl - 1);
}

static void ldd_a_indirect_hl(struct gb *gb)
{
    uint16_t hl = get_r16(gb, R16_HL);
    uint8_t hl_val = cpu_read(gb, hl);

    set_r8(gb, R8_A, hl_val);
    set_r16(gb, R16_HL, hl - 1);
}

static void ldi_indirect_hl_a(struct gb *gb)
{
    uint16_t hl = get_r16(gb, R16_HL);

    cpu_write(gb, hl, get_r8(gb, R8_A));
    set_r16(gb, R16_HL, hl + 1);
}

static void ldi_a_indirect_hl(struct gb *gb)
{
    uint16_t hl = get_r16(gb, R16_HL);

    set_r8(gb, R8_A, cpu_read(gb, hl));
    set_r16(gb, R16_HL, hl + 1);
}

static void ldh_r_a(struct gb *gb, cpu_r8_t r)
{
    uint16_t addr = 0xff00 + get_r8(gb, r);

    mmu_write(gb, addr, get_r8(gb, R8_A));
}

static void ld_r_indirect_hl(struct gb *gb, cpu_r8_t r)
{
    uint16_t hl = get_r16(gb, R16_HL);
    uint8_t hl_val = cpu_read(gb, hl);

    set_r8(gb, r, hl_val);
}

static void ld_indirect_hl_r(struct gb *gb, cpu_r8_t r)
{
    uint16_t hl = get_r16(gb, R16_HL);

    mmu_write(gb, hl, get_r8(gb, r));
}

static void ld_indirect_hl_n(struct gb *gb)
{
    uint8_t n = cpu_read(gb, gb->cpu.regs.pc++);
    uint16_t hl = get_r16(gb, R16_HL);

    mmu_write(gb, hl, n);
}

static void ld_a_indirect_rr(struct gb *gb, cpu_r16_t rr)
{
    uint16_t rr_val = get_r16(gb, rr);

    gb->cpu.regs.a = cpu_read(gb, rr_val);
}

static void ldh_n_a(struct gb *gb)
{
    uint16_t addr = 0xff00 + cpu_read(gb, gb->cpu.regs.pc++);

    mmu_write(gb, addr, get_r8(gb, R8_A));
}

static void ldh_a_n(struct gb *gb)
{
    uint8_t n = cpu_read(gb, gb->cpu.regs.pc++);

    gb->cpu.regs.a = cpu_read(gb, 0xff00 + n);
}

static void ldh_a_r(struct gb *gb, cpu_r8_t r)
{
    uint8_t r_val = get_r8(gb, r);

    gb->cpu.regs.a = cpu_read(gb, 0xff00 + r_val);
}

/****************************/
/****************************/
/* 16-bit Load Instructions */
/****************************/
/****************************/

static void ld_sp_hl(struct gb *gb)
{
    uint16_t hl = get_r16(gb, R16_HL);

    gb->cpu.regs.sp = hl;
}

static void ld_rr_nn(struct gb *gb, cpu_r16_t rr)
{
    uint8_t lsb = cpu_read(gb, gb->cpu.regs.pc++);
    uint8_t msb = cpu_read(gb, gb->cpu.regs.pc++);

    set_r16(gb, rr, TO_U16(lsb, msb));
}

static void ld_indirect_nn_sp(struct gb *gb)
{
    uint8_t lsb = cpu_read(gb, gb->cpu.regs.pc++);
    uint8_t msb = cpu_read(gb, gb->cpu.regs.pc++);
    uint16_t addr = TO_U16(lsb, msb);

    cpu_write(gb, addr, LSB(gb->cpu.regs.sp));
    cpu_write(gb, addr + 1, MSB(gb->cpu.regs.sp));
}

static void push_rr(struct gb *gb, cpu_r16_t rr)
{
    cpu_cycle(gb);
    stack_push(gb, get_r16(gb, rr));
}

static void pop_rr(struct gb *gb, cpu_r16_t rr)
{
    uint16_t val = stack_pop(gb);

    set_r16(gb, rr, val);
}

/***************************************/
/***************************************/
/* 8-bit Arithmetic/Logic Instructions */
/***************************************/
/***************************************/

static void inc_r(struct gb *gb, cpu_r8_t r)
{
    uint8_t r_val = get_r8(gb, r);

    set_r8(gb, r, r_val + 1);
    toggle_flag(gb, FLAG_Z, (uint8_t)(r_val + 1) == 0x00);
    reset_flag(gb, FLAG_N);
    toggle_flag(gb, FLAG_H, ((r_val & 0xf) + (1 & 0xf)) & 0x10);
}

static void dec_r(struct gb *gb, cpu_r8_t r)
{
    uint8_t r_val = get_r8(gb, r);

    set_r8(gb, r, r_val - 1);
    toggle_flag(gb, FLAG_Z, (uint8_t)(r_val - 1) == 0x00);
    set_flag(gb, FLAG_N);
    toggle_flag(gb, FLAG_H, ((r_val & 0xf) - (1 & 0xf)) & 0x10);
}

static void add_a_r(struct gb *gb, cpu_r8_t r)
{
    uint8_t r_val = get_r8(gb, r);
    uint8_t a = get_r8(gb, R8_A);

    set_r8(gb, R8_A, a + r_val);
    toggle_flag(gb, FLAG_Z, (uint8_t)(a + r_val) == 0x00);
    reset_flag(gb, FLAG_N);
    toggle_flag(gb, FLAG_H, ((a & 0xf) + (r_val & 0xf)) & 0x10);
    toggle_flag(gb, FLAG_C, ((a & 0xff) + (r_val & 0xff)) & 0x100);
}

static void add_a_n(struct gb *gb)
{
    uint8_t n = cpu_read(gb, gb->cpu.regs.pc++);
    uint8_t a = get_r8(gb, R8_A);

    set_r8(gb, R8_A, a + n);
    toggle_flag(gb, FLAG_Z, (uint8_t)(a + n) == 0x00);
    reset_flag(gb, FLAG_N);
    toggle_flag(gb, FLAG_H, ((a & 0xf) + (n & 0xf)) & 0x10);
    toggle_flag(gb, FLAG_C, ((a & 0xff) + (n & 0xff)) & 0x100);
}

static void add_a_indirect_hl(struct gb *gb)
{
    uint16_t hl = get_r16(gb, R16_HL);
    uint8_t hl_val = cpu_read(gb, hl);
    uint8_t a = gb->cpu.regs.a;

    set_r8(gb, R8_A, a + hl_val);
    toggle_flag(gb, FLAG_Z, (uint8_t)(a + hl_val) == 0x00);
    reset_flag(gb, FLAG_N);
    toggle_flag(gb, FLAG_H, ((a & 0xf) + (hl_val & 0xf)) & 0x10);
    toggle_flag(gb, FLAG_C, ((a & 0xff) + (hl_val & 0xff)) & 0x100);
}

static void adc_a_r(struct gb *gb, cpu_r8_t r)
{
    uint8_t r_val = get_r8(gb, r);
    uint8_t a = get_r8(gb, R8_A);
    uint8_t old_c = get_flag(gb, FLAG_C);

    set_r8(gb, R8_A, a + r_val + old_c);
    toggle_flag(gb, FLAG_Z, (uint8_t)(a + r_val + old_c) == 0x00);
    reset_flag(gb, FLAG_N);
    toggle_flag(gb, FLAG_H, ((a & 0xf) + (r_val & 0xf) + (old_c & 0xf)) & 0x10);
    toggle_flag(gb, FLAG_C, ((a & 0xff) + (r_val & 0xff) + (old_c & 0xff)) & 0x100);
}

static void adc_a_n(struct gb *gb)
{
    uint8_t n = cpu_read(gb, gb->cpu.regs.pc++);
    uint8_t a = get_r8(gb, R8_A);
    uint8_t old_c = get_flag(gb, FLAG_C);

    set_r8(gb, R8_A, a + n + old_c);
    toggle_flag(gb, FLAG_Z, (uint8_t)(a + n + old_c) == 0x00);
    reset_flag(gb, FLAG_N);
    toggle_flag(gb, FLAG_H, ((a & 0xf) + (n & 0xf) + (old_c & 0xf)) & 0x10);
    toggle_flag(gb, FLAG_C, ((a & 0xff) + (n & 0xff) + (old_c & 0xff)) & 0x100);
}

static void adc_a_indirect_hl(struct gb *gb)
{
    uint16_t hl = get_r16(gb, R16_HL);
    uint8_t hl_val = cpu_read(gb, hl);
    uint8_t a = gb->cpu.regs.a;
    uint8_t old_c = get_flag(gb, FLAG_C);

    set_r8(gb, R8_A, a + hl_val + old_c);
    toggle_flag(gb, FLAG_Z, (uint8_t)(a + hl_val + old_c) == 0x00);
    reset_flag(gb, FLAG_N);
    toggle_flag(gb, FLAG_H, ((a & 0xf) + (hl_val & 0xf) + (old_c & 0xf)) & 0x10);
    toggle_flag(gb, FLAG_C, ((a & 0xff) + (hl_val & 0xff) + (old_c & 0xff)) & 0x100);
}

static void sub_a_r(struct gb *gb, cpu_r8_t r)
{
    uint8_t r_val = get_r8(gb, r);
    uint8_t a = get_r8(gb, R8_A);

    set_r8(gb, R8_A, a - r_val);
    toggle_flag(gb, FLAG_Z, (uint8_t)(a - r_val) == 0x00);
    set_flag(gb, FLAG_N);
    toggle_flag(gb, FLAG_H, ((a & 0xf) - (r_val & 0xf)) & 0x10);
    toggle_flag(gb, FLAG_C, ((a & 0xff) - (r_val & 0xff)) & 0x100);
}

static void sub_a_n(struct gb *gb)
{
    uint8_t n = cpu_read(gb, gb->cpu.regs.pc++);
    uint8_t a = get_r8(gb, R8_A);

    set_r8(gb, R8_A, a - n);
    toggle_flag(gb, FLAG_Z, (uint8_t)(a - n) == 0x00);
    set_flag(gb, FLAG_N);
    toggle_flag(gb, FLAG_H, ((a & 0xf) - (n & 0xf)) & 0x10);
    toggle_flag(gb, FLAG_C, ((a & 0xff) - (n & 0xff)) & 0x100);
}

static void sub_a_indirect_hl(struct gb *gb)
{
    uint16_t hl = get_r16(gb, R16_HL);
    uint8_t hl_val = cpu_read(gb, hl);
    uint8_t a = gb->cpu.regs.a;

    set_r8(gb, R8_A, a - hl_val);
    toggle_flag(gb, FLAG_Z, (uint8_t)(a - hl_val) == 0x00);
    set_flag(gb, FLAG_N);
    toggle_flag(gb, FLAG_H, ((a & 0xf) - (hl_val & 0xf)) & 0x10);
    toggle_flag(gb, FLAG_C, ((a & 0xff) - (hl_val & 0xff)) & 0x100);
}

static void sbc_a_r(struct gb *gb, cpu_r8_t r)
{
    uint8_t r_val = get_r8(gb, r);
    uint8_t a = get_r8(gb, R8_A);
    uint8_t old_c = get_flag(gb, FLAG_C);

    set_r8(gb, R8_A, a - r_val - old_c);
    toggle_flag(gb, FLAG_Z, (uint8_t)(a - r_val - old_c) == 0x00);
    set_flag(gb, FLAG_N);
    toggle_flag(gb, FLAG_H, ((a & 0xf) - (r_val & 0xf) - (old_c & 0xf)) & 0x10);
    toggle_flag(gb, FLAG_C, ((a & 0xff) - (r_val & 0xff) - (old_c & 0xff)) & 0x100);
}

static void sbc_a_n(struct gb *gb)
{
    uint8_t n = cpu_read(gb, gb->cpu.regs.pc++);
    uint8_t a = get_r8(gb, R8_A);
    uint8_t old_c = get_flag(gb, FLAG_C);

    set_r8(gb, R8_A, a - n - old_c);
    toggle_flag(gb, FLAG_Z, (uint8_t)(a - n - old_c) == 0x00);
    set_flag(gb, FLAG_N);
    toggle_flag(gb, FLAG_H, ((a & 0xf) - (n & 0xf) - (old_c & 0xf)) & 0x10);
    toggle_flag(gb, FLAG_C, ((a & 0xff) - (n & 0xff) - (old_c & 0xff)) & 0x100);
}

static void sbc_a_indirect_hl(struct gb *gb)
{
    uint16_t hl = get_r16(gb, R16_HL);
    uint8_t hl_val = cpu_read(gb, hl);
    uint8_t a = gb->cpu.regs.a;
    uint8_t old_c = get_flag(gb, FLAG_C);

    set_r8(gb, R8_A, a - hl_val - old_c);
    toggle_flag(gb, FLAG_Z, (uint8_t)(a - hl_val - old_c) == 0x00);
    set_flag(gb, FLAG_N);
    toggle_flag(gb, FLAG_H, ((a & 0xf) - (hl_val & 0xf) - (old_c & 0xf)) & 0x10);
    toggle_flag(gb, FLAG_C, ((a & 0xff) - (hl_val & 0xff) - (old_c & 0xff)) & 0x100);
}

static void and_a_r(struct gb *gb, cpu_r8_t r)
{
    uint8_t r_val = get_r8(gb, r);
    uint8_t a = get_r8(gb, R8_A);

    set_r8(gb, R8_A, a & r_val);
    toggle_flag(gb, FLAG_Z, (uint8_t)(a & r_val) == 0x00);
    reset_flag(gb, FLAG_N);
    set_flag(gb, FLAG_H);
    reset_flag(gb, FLAG_C);
}

static void and_a_n(struct gb *gb)
{
    uint8_t n = cpu_read(gb, gb->cpu.regs.pc++);
    uint8_t a = get_r8(gb, R8_A);

    set_r8(gb, R8_A, a & n);
    toggle_flag(gb, FLAG_Z, (uint8_t)(a & n) == 0x00);
    reset_flag(gb, FLAG_N);
    set_flag(gb, FLAG_H);
    reset_flag(gb, FLAG_C);
}

static void and_a_indirect_hl(struct gb *gb)
{
    uint16_t hl = get_r16(gb, R16_HL);
    uint8_t hl_val = cpu_read(gb, hl);
    uint8_t a = gb->cpu.regs.a;

    set_r8(gb, R8_A, a & hl_val);
    toggle_flag(gb, FLAG_Z, (uint8_t)(a & hl_val) == 0x00);
    reset_flag(gb, FLAG_N);
    set_flag(gb, FLAG_H);
    reset_flag(gb, FLAG_C);
}

static void xor_a_r(struct gb *gb, cpu_r8_t r)
{
    uint8_t r_val = get_r8(gb, r);
    uint8_t a = get_r8(gb, R8_A);

    set_r8(gb, R8_A, a ^ r_val);
    toggle_flag(gb, FLAG_Z, (uint8_t)(a ^ r_val) == 0x00);
    reset_flag(gb, FLAG_N);
    reset_flag(gb, FLAG_H);
    reset_flag(gb, FLAG_C);
}

static void xor_a_n(struct gb *gb)
{
    uint8_t n = cpu_read(gb, gb->cpu.regs.pc++);
    uint8_t a = get_r8(gb, R8_A);

    set_r8(gb, R8_A, a ^ n);
    toggle_flag(gb, FLAG_Z, (uint8_t)(a ^ n) == 0x00);
    reset_flag(gb, FLAG_N);
    reset_flag(gb, FLAG_H);
    reset_flag(gb, FLAG_C);
}

static void xor_a_indirect_hl(struct gb *gb)
{
    uint16_t hl = get_r16(gb, R16_HL);
    uint8_t hl_val = cpu_read(gb, hl);
    uint8_t a = gb->cpu.regs.a;

    set_r8(gb, R8_A, a ^ hl_val);
    toggle_flag(gb, FLAG_Z, (uint8_t)(a ^ hl_val) == 0x00);
    reset_flag(gb, FLAG_N);
    reset_flag(gb, FLAG_H);
    reset_flag(gb, FLAG_C);
}

static void or_a_r(struct gb *gb, cpu_r8_t r)
{
    uint8_t r_val = get_r8(gb, r);
    uint8_t a = get_r8(gb, R8_A);

    set_r8(gb, R8_A, a | r_val);
    toggle_flag(gb, FLAG_Z, (uint8_t)(a | r_val) == 0x00);
    reset_flag(gb, FLAG_N);
    reset_flag(gb, FLAG_H);
    reset_flag(gb, FLAG_C);
}

static void or_a_n(struct gb *gb)
{
    uint8_t n = cpu_read(gb, gb->cpu.regs.pc++);
    uint8_t a = get_r8(gb, R8_A);

    set_r8(gb, R8_A, a | n);
    toggle_flag(gb, FLAG_Z, (uint8_t)(a | n) == 0x00);
    reset_flag(gb, FLAG_N);
    reset_flag(gb, FLAG_H);
    reset_flag(gb, FLAG_C);
}

static void or_a_indirect_hl(struct gb *gb)
{
    uint16_t hl = get_r16(gb, R16_HL);
    uint8_t hl_val = cpu_read(gb, hl);
    uint8_t a = gb->cpu.regs.a;

    set_r8(gb, R8_A, a | hl_val);
    toggle_flag(gb, FLAG_Z, (uint8_t)(a | hl_val) == 0x00);
    reset_flag(gb, FLAG_N);
    reset_flag(gb, FLAG_H);
    reset_flag(gb, FLAG_C);
}

static void cp_a_r(struct gb *gb, cpu_r8_t r)
{
    uint8_t r_val = get_r8(gb, r);
    uint8_t a = get_r8(gb, R8_A);

    toggle_flag(gb, FLAG_Z, (uint8_t)(a - r_val) == 0x00);
    set_flag(gb, FLAG_N);
    toggle_flag(gb, FLAG_H, ((a & 0xf) - (r_val & 0xf)) & 0x10);
    toggle_flag(gb, FLAG_C, ((a & 0xff) - (r_val & 0xff)) & 0x100);
}

static void cp_a_n(struct gb *gb)
{
    uint8_t n = cpu_read(gb, gb->cpu.regs.pc++);
    uint8_t a = get_r8(gb, R8_A);

    toggle_flag(gb, FLAG_Z, (uint8_t)(a - n) == 0x00);
    set_flag(gb, FLAG_N);
    toggle_flag(gb, FLAG_H,  ((a & 0xf) - (n & 0xf)) & 0x10);
    toggle_flag(gb, FLAG_C, ((a & 0xff) - (n & 0xff)) & 0x100);
}

static void cp_a_indirect_hl(struct gb *gb)
{
    uint16_t hl = get_r16(gb, R16_HL);
    uint8_t hl_val = cpu_read(gb, hl);
    uint8_t a = gb->cpu.regs.a;

    toggle_flag(gb, FLAG_Z, (uint8_t)(a - hl_val) == 0x00);
    set_flag(gb, FLAG_N);
    toggle_flag(gb, FLAG_H, ((a & 0xf) - (hl_val & 0xf)) & 0x10);
    toggle_flag(gb, FLAG_C, ((a & 0xff) - (hl_val & 0xff)) & 0x100);
}

static void inc_indirect_hl(struct gb *gb)
{
    uint16_t hl = get_r16(gb, R16_HL);
    uint8_t hl_val = cpu_read(gb, hl);

    cpu_cycle(gb);
    cpu_write(gb, hl, hl_val + 1);
    toggle_flag(gb, FLAG_Z, (uint8_t)(hl_val + 1) == 0x00);
    reset_flag(gb, FLAG_N);
    toggle_flag(gb, FLAG_H, ((hl_val & 0xf) + (1 & 0xf)) & 0x10);
}

static void dec_indirect_hl(struct gb *gb)
{
    uint16_t hl = get_r16(gb, R16_HL);
    uint8_t hl_val = cpu_read(gb, hl);

    cpu_cycle(gb);
    cpu_write(gb, hl, hl_val - 1);
    toggle_flag(gb, FLAG_Z, (uint8_t)(hl_val - 1) == 0x00);
    set_flag(gb, FLAG_N);
    toggle_flag(gb, FLAG_H, ((hl_val & 0xf) - (1 & 0xf)) & 0x10);
}

static void daa(struct gb *gb)
{
    uint8_t a = gb->cpu.regs.a;

    if (!(gb->cpu.regs.f & FLAG_N)) {  // after an addition, adjust if (half-)carry occurred or if result is out of bounds
        if ((gb->cpu.regs.f & FLAG_C) || a > 0x99) { a += 0x60; gb->cpu.regs.f |= FLAG_C; }
        if ((gb->cpu.regs.f & FLAG_H) || (a & 0x0f) > 0x09) { a += 0x6; }
    } else {  // after a subtraction, only adjust if (half-)carry occurred
        if (gb->cpu.regs.f & FLAG_C) { a -= 0x60; }
        if (gb->cpu.regs.f & FLAG_H) { a -= 0x6; }
    }
    // these flags are always updated
    gb->cpu.regs.f = (a == 0) ? gb->cpu.regs.f | FLAG_Z : gb->cpu.regs.f & ~FLAG_Z; // the usual z flag
    gb->cpu.regs.f &= ~FLAG_H; // h flag is always cleared
    set_r8(gb, R8_A, a);
}

static void cpl(struct gb *gb)
{
    gb->cpu.regs.a ^= 0xff;
    set_flag(gb, FLAG_N);
    set_flag(gb, FLAG_H);
}

/****************************************/
/****************************************/
/* 16-bit Arithmetic/Logic Instructions */
/****************************************/
/****************************************/

static void add_sp_i8(struct gb *gb)
{
    uint8_t i8 = cpu_read(gb, gb->cpu.regs.pc++);
    uint16_t sp = gb->cpu.regs.sp;

    cpu_cycle(gb);
    gb->cpu.regs.sp += (int8_t)i8;
    reset_flag(gb, FLAG_Z);
    reset_flag(gb, FLAG_N);
    toggle_flag(gb, FLAG_H, ((sp & 0xf) + (i8 & 0xf)) & 0x10);
    toggle_flag(gb, FLAG_C, ((sp & 0xff) + (i8 & 0xff)) & 0x100);
}

static void ld_hl_sp_plus_i8(struct gb *gb)
{
    uint8_t i8 = cpu_read(gb, gb->cpu.regs.pc++);
    uint16_t sp = gb->cpu.regs.sp;

    cpu_cycle(gb);
    set_r16(gb, R16_HL, sp + (int8_t)i8);
    reset_flag(gb, FLAG_Z);
    reset_flag(gb, FLAG_N);
    toggle_flag(gb, FLAG_H, ((sp & 0xf) + (i8 & 0xf)) & 0x10);
    toggle_flag(gb, FLAG_C, ((sp & 0xff) + (i8 & 0xff)) & 0x100);
}

static void add_hl_rr(struct gb *gb, cpu_r16_t rr)
{
    uint32_t hl_val = get_r16(gb, R16_HL);
    uint32_t rr_val = get_r16(gb, rr);

    set_r16(gb, R16_HL, hl_val + rr_val);
    reset_flag(gb, FLAG_N);
    toggle_flag(gb, FLAG_H, ((hl_val & 0xfff) + (rr_val & 0xfff)) & 0x1000);
    toggle_flag(gb, FLAG_C, ((hl_val & 0xffff) + (rr_val & 0xffff)) & 0x10000);
}

static void inc_rr(struct gb *gb, cpu_r16_t rr)
{
    uint16_t rr_val = get_r16(gb, rr);

    set_r16(gb, rr, rr_val + 1);
}

static void dec_rr(struct gb *gb, cpu_r16_t rr)
{
    uint16_t rr_val = get_r16(gb, rr);

    set_r16(gb, rr, rr_val - 1);
}

/*************************************/
/*************************************/
/* Single-bit Operation Instructions */
/*************************************/
/*************************************/

static void bit_n_r(struct gb *gb, uint8_t n, cpu_r8_t r)
{
    toggle_flag(gb, FLAG_Z, (get_r8(gb, r) & (1U << n)) == 0x00);
    reset_flag(gb, FLAG_N);
    set_flag(gb, FLAG_H);
}

static void bit_n_indirect_hl(struct gb *gb, uint8_t n)
{
    uint8_t hl_val = cpu_read(gb, get_r16(gb, R16_HL));

    toggle_flag(gb, FLAG_Z, (hl_val & (1U << n)) == 0x00);
    reset_flag(gb, FLAG_N);
    set_flag(gb, FLAG_H);
}

static void set_n_r(struct gb *gb, uint8_t n, cpu_r8_t r)
{
    uint8_t r_val = get_r8(gb, r);

    set_r8(gb, r, r_val | (1U << n));
}

static void set_n_indirect_hl(struct gb *gb, uint8_t n)
{
    uint16_t hl = get_r16(gb, R16_HL);
    uint8_t hl_val = cpu_read(gb, hl);

    cpu_write(gb, hl, hl_val | (1U << n));
}

static void res_n_r(struct gb *gb, uint8_t n, cpu_r8_t r)
{
    uint8_t r_val = get_r8(gb, r);

    set_r8(gb, r, r_val & ~(1U << n));
}

static void res_n_indirect_hl(struct gb *gb, uint8_t n)
{
    uint16_t hl = get_r16(gb, R16_HL);
    uint8_t hl_val = cpu_read(gb, get_r16(gb, R16_HL));

    cpu_write(gb, hl, hl_val & ~(1U << n));
}

/*********************/
/*********************/
/* Jump Instructions */
/*********************/
/*********************/

static void reti(struct gb *gb)
{
    uint16_t pc = stack_pop(gb);
    cpu_cycle(gb);
    gb->cpu.regs.pc = pc;
    // TODO: set ime = 1 here
}

static void rst_n(struct gb *gb, uint16_t addr)
{
    cpu_cycle(gb);
    stack_push(gb, gb->cpu.regs.pc);
    gb->cpu.regs.pc = addr;
}

static void ret(struct gb *gb)
{
    gb->cpu.regs.pc = stack_pop(gb);
    cpu_cycle(gb);
}

static void ret_f(struct gb *gb, cpu_cond_t cond)
{
    cpu_cycle(gb);
    if (check_cond(gb, cond)) {
        cpu_cycle(gb);
        gb->cpu.regs.pc = stack_pop(gb);
    }
}

static void jp_hl(struct gb *gb)
{
    uint16_t hl = get_r16(gb, R16_HL);

    gb->cpu.regs.pc = hl;
}

static void jp_nn(struct gb *gb)
{
    uint8_t lsb = cpu_read(gb, gb->cpu.regs.pc++);
    uint8_t msb = cpu_read(gb, gb->cpu.regs.pc++);

    cpu_cycle(gb);
    gb->cpu.regs.pc = TO_U16(lsb, msb);
}

static void jp_f_nn(struct gb *gb, cpu_cond_t cond)
{
    uint8_t lsb = cpu_read(gb, gb->cpu.regs.pc++);
    uint8_t msb = cpu_read(gb, gb->cpu.regs.pc++);

    if (check_cond(gb, cond)) {
        cpu_cycle(gb);
        gb->cpu.regs.pc = TO_U16(lsb, msb);
    }
}

static void jr_f_i8(struct gb *gb, cpu_cond_t cond)
{
    uint8_t i8 = cpu_read(gb, gb->cpu.regs.pc++);

    if (check_cond(gb, cond)) {
        cpu_cycle(gb);
        gb->cpu.regs.pc += (int8_t)i8;
    }
}

static void jr_i8(struct gb *gb)
{
    uint8_t i8 = cpu_read(gb, gb->cpu.regs.pc++);

    cpu_cycle(gb);
    gb->cpu.regs.pc += (int8_t)i8;
}

static void call_nn(struct gb *gb)
{
    uint8_t lsb = cpu_read(gb, gb->cpu.regs.pc++);
    uint8_t msb = cpu_read(gb, gb->cpu.regs.pc++);

    cpu_cycle(gb);
    stack_push(gb, gb->cpu.regs.pc);
    gb->cpu.regs.pc = TO_U16(lsb, msb);
}

static void call_f_nn(struct gb *gb, cpu_cond_t cond)
{
    uint8_t lsb = cpu_read(gb, gb->cpu.regs.pc++);
    uint8_t msb = cpu_read(gb, gb->cpu.regs.pc++);

    if (check_cond(gb, cond)) {
        cpu_cycle(gb);
        stack_push(gb, gb->cpu.regs.pc);
        gb->cpu.regs.pc = TO_U16(lsb, msb);
    }
}

/*********************************/
/*********************************/
/* Rotate and Shift Instructions */
/*********************************/
/*********************************/

static void rlca(struct gb *gb)
{
    uint8_t new_c = gb->cpu.regs.a & (1U << 7);

    gb->cpu.regs.a = (gb->cpu.regs.a << 1) | (new_c >> 7);
    reset_flag(gb, FLAG_Z);
    reset_flag(gb, FLAG_N);
    reset_flag(gb, FLAG_H);
    toggle_flag(gb, FLAG_C, new_c);
}

static void rrca(struct gb *gb)
{
    uint8_t new_c = gb->cpu.regs.a & (1U << 0);

    gb->cpu.regs.a = (gb->cpu.regs.a >> 1) | (new_c << 7);
    reset_flag(gb, FLAG_Z);
    reset_flag(gb, FLAG_N);
    reset_flag(gb, FLAG_H);
    toggle_flag(gb, FLAG_C, new_c);
}

static void rlc_r(struct gb *gb, cpu_r8_t r)
{
    uint8_t r_val = get_r8(gb, r);
    uint8_t new_c = (r_val & 0x80) >> 7;

    set_r8(gb, r, (r_val << 1) | new_c);
    toggle_flag(gb, FLAG_Z, get_r8(gb, r) == 0x00);
    reset_flag(gb, FLAG_N);
    reset_flag(gb, FLAG_H);
    toggle_flag(gb, FLAG_C, new_c);
}

static void rlc_indirect_hl(struct gb *gb)
{
    uint16_t hl = get_r16(gb, R16_HL);
    uint8_t hl_val = cpu_read(gb, hl);
    uint8_t new_c = (hl_val & 0x80) >> 7;

    cpu_write(gb, hl, (hl_val << 1) | new_c);
    toggle_flag(gb, FLAG_Z, (uint8_t)((hl_val << 1) | new_c) == 0x00);
    reset_flag(gb, FLAG_N);
    reset_flag(gb, FLAG_H);
    toggle_flag(gb, FLAG_C, new_c);
}

static void rrc_r(struct gb *gb, cpu_r8_t r)
{
    uint8_t r_val = get_r8(gb, r);
    uint8_t new_c = (r_val & 0x01) << 7;

    set_r8(gb, r, (r_val >> 1) | new_c);
    toggle_flag(gb, FLAG_Z, get_r8(gb, r) == 0x00);
    reset_flag(gb, FLAG_N);
    reset_flag(gb, FLAG_H);
    toggle_flag(gb, FLAG_C, new_c);
}

static void rrc_indirect_hl(struct gb *gb)
{
    uint16_t hl = get_r16(gb, R16_HL);
    uint8_t hl_val = cpu_read(gb, hl);
    uint8_t new_c = (hl_val & 0x01) << 7;

    cpu_write(gb, hl, (hl_val >> 1) | new_c);
    toggle_flag(gb, FLAG_Z, (uint8_t)((hl_val >> 1) | new_c) == 0x00);
    reset_flag(gb, FLAG_N);
    reset_flag(gb, FLAG_H);
    toggle_flag(gb, FLAG_C, new_c);
}

static void rl_r(struct gb *gb, cpu_r8_t r)
{
    uint8_t r_val = get_r8(gb, r);
    uint8_t old_c = get_flag(gb, FLAG_C), new_c = r_val & 0x80;

    set_r8(gb, r, (r_val << 1) | old_c);

    toggle_flag(gb, FLAG_Z, (uint8_t)((r_val << 1) | old_c) == 0x00);
    reset_flag(gb, FLAG_N);
    reset_flag(gb, FLAG_H);
    toggle_flag(gb, FLAG_C, new_c);
}

static void rl_indirect_hl(struct gb *gb)
{
    uint16_t hl = get_r16(gb, R16_HL);
    uint8_t hl_val = cpu_read(gb, hl);
    uint8_t old_c = get_flag(gb, FLAG_C), new_c = hl_val & 0x80;

    cpu_write(gb, hl, (hl_val << 1) | old_c);

    toggle_flag(gb, FLAG_Z, (uint8_t)((hl_val << 1) | old_c) == 0x00);
    reset_flag(gb, FLAG_N);
    reset_flag(gb, FLAG_H);
    toggle_flag(gb, FLAG_C, new_c);
}

static void rr_r(struct gb *gb, cpu_r8_t r)
{
    uint8_t r_val = get_r8(gb, r);
    uint8_t old_c = get_flag(gb, FLAG_C), new_c = r_val & 0x01;

    set_r8(gb, r, (r_val >> 1) | (old_c << 7));

    toggle_flag(gb, FLAG_Z, ((r_val >> 1) | (old_c << 7)) == 0x00);
    reset_flag(gb, FLAG_N);
    reset_flag(gb, FLAG_H);
    toggle_flag(gb, FLAG_C, new_c);
}

static void rr_indirect_hl(struct gb *gb)
{
    uint16_t hl = get_r16(gb, R16_HL);
    uint8_t hl_val = cpu_read(gb, hl);
    uint8_t old_c = get_flag(gb, FLAG_C), new_c = hl_val & 0x01;

    cpu_write(gb, hl, (hl_val >> 1) | (old_c << 7));

    toggle_flag(gb, FLAG_Z, (uint8_t)((hl_val >> 1) | (old_c << 7)) == 0x00);
    reset_flag(gb, FLAG_N);
    reset_flag(gb, FLAG_H);
    toggle_flag(gb, FLAG_C, new_c);
}

static void sla_r(struct gb *gb, cpu_r8_t r)
{
    uint8_t r_val = get_r8(gb, r);
    uint8_t new_c = r_val & 0x80;

    set_r8(gb, r, r_val << 1);
    toggle_flag(gb, FLAG_Z, get_r8(gb, r) == 0x00);
    reset_flag(gb, FLAG_N);
    reset_flag(gb, FLAG_H);
    toggle_flag(gb, FLAG_C, new_c);
}

static void sla_indirect_hl(struct gb *gb)
{
    uint16_t hl = get_r16(gb, R16_HL);
    uint8_t hl_val = cpu_read(gb, hl);
    uint8_t new_c = hl_val & 0x80;

    cpu_write(gb, hl, hl_val << 1);
    toggle_flag(gb, FLAG_Z, (uint8_t)(hl_val << 1) == 0x00);
    reset_flag(gb, FLAG_N);
    reset_flag(gb, FLAG_H);
    toggle_flag(gb, FLAG_C, new_c);
}

static void sra_r(struct gb *gb, cpu_r8_t r)
{
    uint8_t r_val = get_r8(gb, r);
    uint8_t new_c = r_val & 0x01;

    set_r8(gb, r, (r_val >> 1) | (r_val & 0x80));
    toggle_flag(gb, FLAG_Z, get_r8(gb, r) == 0x00);
    reset_flag(gb, FLAG_N);
    reset_flag(gb, FLAG_H);
    toggle_flag(gb, FLAG_C, new_c);
}

static void sra_indirect_hl(struct gb *gb)
{
    uint16_t hl = get_r16(gb, R16_HL);
    uint8_t hl_val = cpu_read(gb, hl);
    uint8_t new_c = hl_val & 0x01;

    cpu_write(gb, hl, (hl_val >> 1) | (hl_val & 0x80));
    toggle_flag(gb, FLAG_Z, (uint8_t)((hl_val >> 1) | (hl_val & 0x80)) == 0x00);
    reset_flag(gb, FLAG_N);
    reset_flag(gb, FLAG_H);
    toggle_flag(gb, FLAG_C, new_c);
}

static void swap_r(struct gb *gb, cpu_r8_t r)
{
    uint8_t r_val = get_r8(gb, r);

    set_r8(gb, r, (r_val << 4) | (r_val >> 4));
    toggle_flag(gb, FLAG_Z, get_r8(gb, r) == 0x00);
    reset_flag(gb, FLAG_N);
    reset_flag(gb, FLAG_H);
    reset_flag(gb, FLAG_C);
}

static void swap_indirect_hl(struct gb *gb)
{
    uint16_t hl = get_r16(gb, R16_HL);
    uint8_t hl_val = cpu_read(gb, hl);

    cpu_write(gb, hl, (hl_val << 4) | (hl_val >> 4));
    toggle_flag(gb, FLAG_Z, (uint8_t)((hl_val << 4) | (hl_val >> 4)) == 0x00);
    reset_flag(gb, FLAG_N);
    reset_flag(gb, FLAG_H);
    reset_flag(gb, FLAG_C);
}

static void srl_r(struct gb *gb, cpu_r8_t r)
{
    uint8_t r_val = get_r8(gb, r);
    uint8_t new_c = r_val & 0x01;

    set_r8(gb, r, r_val >> 1);
    toggle_flag(gb, FLAG_Z, (r_val >> 1) == 0x00);
    reset_flag(gb, FLAG_N);
    reset_flag(gb, FLAG_H);
    toggle_flag(gb, FLAG_C, new_c);
}

static void srl_indirect_hl(struct gb *gb)
{
    uint16_t hl = get_r16(gb, R16_HL);
    uint8_t hl_val = cpu_read(gb, hl);
    uint8_t new_c = hl_val & 0x01;

    cpu_write(gb, hl, hl_val >> 1);
    toggle_flag(gb, FLAG_Z, (hl_val >> 1) == 0x00);
    reset_flag(gb, FLAG_N);
    reset_flag(gb, FLAG_H);
    toggle_flag(gb, FLAG_C, new_c);
}

static void rla(struct gb *gb)
{
    uint8_t a = get_r8(gb, R8_A);
    uint8_t old_c = get_flag(gb, FLAG_C), new_c = a & 0x80;

    set_r8(gb, R8_A, (a << 1) | old_c);

    reset_flag(gb, FLAG_Z);
    reset_flag(gb, FLAG_N);
    reset_flag(gb, FLAG_H);
    toggle_flag(gb, FLAG_C, new_c);
}

static void rra(struct gb *gb)
{
    uint8_t a = get_r8(gb, R8_A);
    uint8_t old_c = get_flag(gb, FLAG_C), new_c = a & 0x01;

    set_r8(gb, R8_A, (a >> 1) | (old_c << 7));

    reset_flag(gb, FLAG_Z);
    reset_flag(gb, FLAG_N);
    reset_flag(gb, FLAG_H);
    toggle_flag(gb, FLAG_C, new_c);
}

/****************************/
/****************************/
/* CPU Control Instructions */
/****************************/
/****************************/

static void ccf(struct gb *gb)
{
    uint8_t cy = get_flag(gb, FLAG_C) ^ 1;

    reset_flag(gb, FLAG_N);
    reset_flag(gb, FLAG_H);
    toggle_flag(gb, FLAG_C, cy);
}

static void scf(struct gb *gb)
{
    reset_flag(gb, FLAG_N);
    reset_flag(gb, FLAG_H);
    set_flag(gb, FLAG_C);
}

// TODO: complete these instructions
static void halt(struct gb *gb)
{
    gb->cpu.mode = HALT;
}

static void stop(struct gb *gb)
{
    gb->cpu.mode = STOP;
}

static void di(struct gb *gb)
{
}

static void ei(struct gb *gb)
{
}

void execute_cb_instructions(struct gb *gb)
{
    uint8_t opcode = cpu_read(gb, gb->cpu.regs.pc++);

    switch (opcode) {
    case 0x00: rlc_r(gb, R8_B);         break;
    case 0x01: rlc_r(gb, R8_C);         break;
    case 0x02: rlc_r(gb, R8_D);         break;
    case 0x03: rlc_r(gb, R8_E);         break;
    case 0x04: rlc_r(gb, R8_H);         break;
    case 0x05: rlc_r(gb, R8_L);         break;
    case 0x06: rlc_indirect_hl(gb);     break;
    case 0x07: rlc_r(gb, R8_A);         break;
    case 0x08: rrc_r(gb, R8_B);         break;
    case 0x09: rrc_r(gb, R8_C);         break;
    case 0x0a: rrc_r(gb, R8_D);         break;
    case 0x0b: rrc_r(gb, R8_E);         break;
    case 0x0c: rrc_r(gb, R8_H);         break;
    case 0x0d: rrc_r(gb, R8_L);         break;
    case 0x0e: rrc_indirect_hl(gb);     break;
    case 0x0f: rrc_r(gb, R8_A);         break;
    case 0x10: rl_r(gb, R8_B);          break;
    case 0x11: rl_r(gb, R8_C);          break;
    case 0x12: rl_r(gb, R8_D);          break;
    case 0x13: rl_r(gb, R8_E);          break;
    case 0x14: rl_r(gb, R8_H);          break;
    case 0x15: rl_r(gb, R8_L);          break;
    case 0x16: rl_indirect_hl(gb);      break;
    case 0x17: rl_r(gb, R8_A);          break;
    case 0x18: rr_r(gb, R8_B);          break;
    case 0x19: rr_r(gb, R8_C);          break;
    case 0x1a: rr_r(gb, R8_D);          break;
    case 0x1b: rr_r(gb, R8_E);          break;
    case 0x1c: rr_r(gb, R8_H);          break;
    case 0x1d: rr_r(gb, R8_L);          break;
    case 0x1e: rr_indirect_hl(gb);      break;
    case 0x1f: rr_r(gb, R8_A);          break;
    case 0x20: sla_r(gb, R8_B);         break;
    case 0x21: sla_r(gb, R8_C);         break;
    case 0x22: sla_r(gb, R8_D);         break;
    case 0x23: sla_r(gb, R8_E);         break;
    case 0x24: sla_r(gb, R8_H);         break;
    case 0x25: sla_r(gb, R8_L);         break;
    case 0x26: sla_indirect_hl(gb);     break;
    case 0x27: sla_r(gb, R8_A);         break;
    case 0x28: sra_r(gb, R8_B);         break;
    case 0x29: sra_r(gb, R8_C);         break;
    case 0x2a: sra_r(gb, R8_D);         break;
    case 0x2b: sra_r(gb, R8_E);         break;
    case 0x2c: sra_r(gb, R8_H);         break;
    case 0x2d: sra_r(gb, R8_L);         break;
    case 0x2e: sra_indirect_hl(gb);     break;
    case 0x2f: sra_r(gb, R8_A);         break;
    case 0x30: swap_r(gb, R8_B);        break;
    case 0x31: swap_r(gb, R8_C);        break;
    case 0x32: swap_r(gb, R8_D);        break;
    case 0x33: swap_r(gb, R8_E);        break;
    case 0x34: swap_r(gb, R8_H);        break;
    case 0x35: swap_r(gb, R8_L);        break;
    case 0x36: swap_indirect_hl(gb);    break;
    case 0x37: swap_r(gb, R8_A);        break;
    case 0x38: srl_r(gb, R8_B);         break;
    case 0x39: srl_r(gb, R8_C);         break;
    case 0x3a: srl_r(gb, R8_D);         break;
    case 0x3b: srl_r(gb, R8_E);         break;
    case 0x3c: srl_r(gb, R8_H);         break;
    case 0x3d: srl_r(gb, R8_L);         break;
    case 0x3e: srl_indirect_hl(gb);     break;
    case 0x3f: srl_r(gb, R8_A);         break;
    case 0x40: bit_n_r(gb, 0, R8_B);    break;
    case 0x41: bit_n_r(gb, 0, R8_C);    break;
    case 0x42: bit_n_r(gb, 0, R8_D);    break;
    case 0x43: bit_n_r(gb, 0, R8_E);    break;
    case 0x44: bit_n_r(gb, 0, R8_H);    break;
    case 0x45: bit_n_r(gb, 0, R8_L);    break;
    case 0x46: bit_n_indirect_hl(gb, 0);    break;
    case 0x47: bit_n_r(gb, 0, R8_A);        break;
    case 0x48: bit_n_r(gb, 1, R8_B);        break;
    case 0x49: bit_n_r(gb, 1, R8_C);        break;
    case 0x4a: bit_n_r(gb, 1, R8_D);        break;
    case 0x4b: bit_n_r(gb, 1, R8_E);        break;
    case 0x4c: bit_n_r(gb, 1, R8_H);        break;
    case 0x4d: bit_n_r(gb, 1, R8_L);        break;
    case 0x4e: bit_n_indirect_hl(gb, 1);        break;
    case 0x4f: bit_n_r(gb, 1, R8_A);        break;
    case 0x50: bit_n_r(gb, 2, R8_B);    break;
    case 0x51: bit_n_r(gb, 2, R8_C);    break;
    case 0x52: bit_n_r(gb, 2, R8_D);    break;
    case 0x53: bit_n_r(gb, 2, R8_E);    break;
    case 0x54: bit_n_r(gb, 2, R8_H);    break;
    case 0x55: bit_n_r(gb, 2, R8_L);    break;
    case 0x56: bit_n_indirect_hl(gb, 2);    break;
    case 0x57: bit_n_r(gb, 2, R8_A);        break;
    case 0x58: bit_n_r(gb, 3, R8_B);        break;
    case 0x59: bit_n_r(gb, 3, R8_C);        break;
    case 0x5a: bit_n_r(gb, 3, R8_D);        break;
    case 0x5b: bit_n_r(gb, 3, R8_E);        break;
    case 0x5c: bit_n_r(gb, 3, R8_H);        break;
    case 0x5d: bit_n_r(gb, 3, R8_L);        break;
    case 0x5e: bit_n_indirect_hl(gb, 3);        break;
    case 0x5f: bit_n_r(gb, 3, R8_A);        break;
    case 0x60: bit_n_r(gb, 4, R8_B);    break;
    case 0x61: bit_n_r(gb, 4, R8_C);    break;
    case 0x62: bit_n_r(gb, 4, R8_D);    break;
    case 0x63: bit_n_r(gb, 4, R8_E);    break;
    case 0x64: bit_n_r(gb, 4, R8_H);    break;
    case 0x65: bit_n_r(gb, 4, R8_L);    break;
    case 0x66: bit_n_indirect_hl(gb, 4);    break;
    case 0x67: bit_n_r(gb, 4, R8_A);        break;
    case 0x68: bit_n_r(gb, 5, R8_B);        break;
    case 0x69: bit_n_r(gb, 5, R8_C);        break;
    case 0x6a: bit_n_r(gb, 5, R8_D);        break;
    case 0x6b: bit_n_r(gb, 5, R8_E);        break;
    case 0x6c: bit_n_r(gb, 5, R8_H);        break;
    case 0x6d: bit_n_r(gb, 5, R8_L);        break;
    case 0x6e: bit_n_indirect_hl(gb, 5);        break;
    case 0x6f: bit_n_r(gb, 5, R8_A);        break;
    case 0x70: bit_n_r(gb, 6, R8_B);    break;
    case 0x71: bit_n_r(gb, 6, R8_C);    break;
    case 0x72: bit_n_r(gb, 6, R8_D);    break;
    case 0x73: bit_n_r(gb, 6, R8_E);    break;
    case 0x74: bit_n_r(gb, 6, R8_H);    break;
    case 0x75: bit_n_r(gb, 6, R8_L);    break;
    case 0x76: bit_n_indirect_hl(gb, 6);    break;
    case 0x77: bit_n_r(gb, 6, R8_A);        break;
    case 0x78: bit_n_r(gb, 7, R8_B);        break;
    case 0x79: bit_n_r(gb, 7, R8_C);        break;
    case 0x7a: bit_n_r(gb, 7, R8_D);        break;
    case 0x7b: bit_n_r(gb, 7, R8_E);        break;
    case 0x7c: bit_n_r(gb, 7, R8_H);        break;
    case 0x7d: bit_n_r(gb, 7, R8_L);        break;
    case 0x7e: bit_n_indirect_hl(gb, 7);        break;
    case 0x7f: bit_n_r(gb, 7, R8_A);        break;
    case 0x80: res_n_r(gb, 0, R8_B);        break;
    case 0x81: res_n_r(gb, 0, R8_C);        break;
    case 0x82: res_n_r(gb, 0, R8_D);        break;
    case 0x83: res_n_r(gb, 0, R8_E);        break;
    case 0x84: res_n_r(gb, 0, R8_H);        break;
    case 0x85: res_n_r(gb, 0, R8_L);        break;
    case 0x86: res_n_indirect_hl(gb, 0);       break;
    case 0x87: res_n_r(gb, 0, R8_A);        break;
    case 0x88: res_n_r(gb, 1, R8_B);        break;
    case 0x89: res_n_r(gb, 1, R8_C);        break;
    case 0x8a: res_n_r(gb, 1, R8_D);        break;
    case 0x8b: res_n_r(gb, 1, R8_E);        break;
    case 0x8c: res_n_r(gb, 1, R8_H);        break;
    case 0x8d: res_n_r(gb, 1, R8_L);        break;
    case 0x8e: res_n_indirect_hl(gb, 1);       break;
    case 0x8f: res_n_r(gb, 1, R8_A);        break;
    case 0x90: res_n_r(gb, 2, R8_B);        break;
    case 0x91: res_n_r(gb, 2, R8_C);        break;
    case 0x92: res_n_r(gb, 2, R8_D);        break;
    case 0x93: res_n_r(gb, 2, R8_E);        break;
    case 0x94: res_n_r(gb, 2, R8_H);        break;
    case 0x95: res_n_r(gb, 2, R8_L);        break;
    case 0x96: res_n_indirect_hl(gb, 2);       break;
    case 0x97: res_n_r(gb, 2, R8_A);        break;
    case 0x98: res_n_r(gb, 3, R8_B);        break;
    case 0x99: res_n_r(gb, 3, R8_C);        break;
    case 0x9a: res_n_r(gb, 3, R8_D);        break;
    case 0x9b: res_n_r(gb, 3, R8_E);        break;
    case 0x9c: res_n_r(gb, 3, R8_H);        break;
    case 0x9d: res_n_r(gb, 3, R8_L);        break;
    case 0x9e: res_n_indirect_hl(gb, 3);       break;
    case 0x9f: res_n_r(gb, 3, R8_A);        break;
    case 0xa0: res_n_r(gb, 4, R8_B);        break;
    case 0xa1: res_n_r(gb, 4, R8_C);        break;
    case 0xa2: res_n_r(gb, 4, R8_D);        break;
    case 0xa3: res_n_r(gb, 4, R8_E);        break;
    case 0xa4: res_n_r(gb, 4, R8_H);        break;
    case 0xa5: res_n_r(gb, 4, R8_L);        break;
    case 0xa6: res_n_indirect_hl(gb, 4);       break;
    case 0xa7: res_n_r(gb, 4, R8_A);        break;
    case 0xa8: res_n_r(gb, 5, R8_B);        break;
    case 0xa9: res_n_r(gb, 5, R8_C);        break;
    case 0xaa: res_n_r(gb, 5, R8_D);        break;
    case 0xab: res_n_r(gb, 5, R8_E);        break;
    case 0xac: res_n_r(gb, 5, R8_H);        break;
    case 0xad: res_n_r(gb, 5, R8_L);        break;
    case 0xae: res_n_indirect_hl(gb, 5);       break;
    case 0xaf: res_n_r(gb, 5, R8_A);        break;
    case 0xb0: res_n_r(gb, 6, R8_B);        break;
    case 0xb1: res_n_r(gb, 6, R8_C);        break;
    case 0xb2: res_n_r(gb, 6, R8_D);        break;
    case 0xb3: res_n_r(gb, 6, R8_E);        break;
    case 0xb4: res_n_r(gb, 6, R8_H);        break;
    case 0xb5: res_n_r(gb, 6, R8_L);        break;
    case 0xb6: res_n_indirect_hl(gb, 6);       break;
    case 0xb7: res_n_r(gb, 6, R8_A);        break;
    case 0xb8: res_n_r(gb, 7, R8_B);        break;
    case 0xb9: res_n_r(gb, 7, R8_C);        break;
    case 0xba: res_n_r(gb, 7, R8_D);        break;
    case 0xbb: res_n_r(gb, 7, R8_E);        break;
    case 0xbc: res_n_r(gb, 7, R8_H);        break;
    case 0xbd: res_n_r(gb, 7, R8_L);        break;
    case 0xbe: res_n_indirect_hl(gb, 7);       break;
    case 0xbf: res_n_r(gb, 7, R8_A);        break;
    case 0xc0: set_n_r(gb, 0, R8_B);        break;
    case 0xc1: set_n_r(gb, 0, R8_C);        break;
    case 0xc2: set_n_r(gb, 0, R8_D);        break;
    case 0xc3: set_n_r(gb, 0, R8_E);        break;
    case 0xc4: set_n_r(gb, 0, R8_H);        break;
    case 0xc5: set_n_r(gb, 0, R8_L);        break;
    case 0xc6: set_n_indirect_hl(gb, 0);       break;
    case 0xc7: set_n_r(gb, 0, R8_A);        break;
    case 0xc8: set_n_r(gb, 1, R8_B);        break;
    case 0xc9: set_n_r(gb, 1, R8_C);        break;
    case 0xca: set_n_r(gb, 1, R8_D);        break;
    case 0xcb: set_n_r(gb, 1, R8_E);        break;
    case 0xcc: set_n_r(gb, 1, R8_H);        break;
    case 0xcd: set_n_r(gb, 1, R8_L);        break;
    case 0xce: set_n_indirect_hl(gb, 1);       break;
    case 0xcf: set_n_r(gb, 1, R8_A);        break;
    case 0xd0: set_n_r(gb, 2, R8_B);        break;
    case 0xd1: set_n_r(gb, 2, R8_C);        break;
    case 0xd2: set_n_r(gb, 2, R8_D);        break;
    case 0xd3: set_n_r(gb, 2, R8_E);        break;
    case 0xd4: set_n_r(gb, 2, R8_H);        break;
    case 0xd5: set_n_r(gb, 2, R8_L);        break;
    case 0xd6: set_n_indirect_hl(gb, 2);       break;
    case 0xd7: set_n_r(gb, 2, R8_A);        break;
    case 0xd8: set_n_r(gb, 3, R8_B);        break;
    case 0xd9: set_n_r(gb, 3, R8_C);        break;
    case 0xda: set_n_r(gb, 3, R8_D);        break;
    case 0xdb: set_n_r(gb, 3, R8_E);        break;
    case 0xdc: set_n_r(gb, 3, R8_H);        break;
    case 0xdd: set_n_r(gb, 3, R8_L);        break;
    case 0xde: set_n_indirect_hl(gb, 3);       break;
    case 0xdf: set_n_r(gb, 3, R8_A);        break;
    case 0xe0: set_n_r(gb, 4, R8_B);        break;
    case 0xe1: set_n_r(gb, 4, R8_C);        break;
    case 0xe2: set_n_r(gb, 4, R8_D);        break;
    case 0xe3: set_n_r(gb, 4, R8_E);        break;
    case 0xe4: set_n_r(gb, 4, R8_H);        break;
    case 0xe5: set_n_r(gb, 4, R8_L);        break;
    case 0xe6: set_n_indirect_hl(gb, 4);       break;
    case 0xe7: set_n_r(gb, 4, R8_A);        break;
    case 0xe8: set_n_r(gb, 5, R8_B);        break;
    case 0xe9: set_n_r(gb, 5, R8_C);        break;
    case 0xea: set_n_r(gb, 5, R8_D);        break;
    case 0xeb: set_n_r(gb, 5, R8_E);        break;
    case 0xec: set_n_r(gb, 5, R8_H);        break;
    case 0xed: set_n_r(gb, 5, R8_L);        break;
    case 0xee: set_n_indirect_hl(gb, 5);       break;
    case 0xef: set_n_r(gb, 5, R8_A);        break;
    case 0xf0: set_n_r(gb, 6, R8_B);        break;
    case 0xf1: set_n_r(gb, 6, R8_C);        break;
    case 0xf2: set_n_r(gb, 6, R8_D);        break;
    case 0xf3: set_n_r(gb, 6, R8_E);        break;
    case 0xf4: set_n_r(gb, 6, R8_H);        break;
    case 0xf5: set_n_r(gb, 6, R8_L);        break;
    case 0xf6: set_n_indirect_hl(gb, 6);       break;
    case 0xf7: set_n_r(gb, 6, R8_A);        break;
    case 0xf8: set_n_r(gb, 7, R8_B);        break;
    case 0xf9: set_n_r(gb, 7, R8_C);        break;
    case 0xfa: set_n_r(gb, 7, R8_D);        break;
    case 0xfb: set_n_r(gb, 7, R8_E);        break;
    case 0xfc: set_n_r(gb, 7, R8_H);        break;
    case 0xfd: set_n_r(gb, 7, R8_L);        break;
    case 0xfe: set_n_indirect_hl(gb, 7);       break;
    case 0xff: set_n_r(gb, 7, R8_A);        break;
    default:
        printf("Instruction CB %02x is invalid\n", opcode);
        exit(EXIT_FAILURE);
    }
}

void execute_normal_instructions(struct gb *gb)
{
    uint8_t opcode = cpu_read(gb, gb->cpu.regs.pc++);

    switch (opcode) {
    case 0x00: break;
    case 0x01: ld_rr_nn(gb, R16_BC);        break;
    case 0x02: ld_indirect_rr_a(gb, R16_BC); break;
    case 0x03: inc_rr(gb, R16_BC);          break;
    case 0x04: inc_r(gb, R8_B);             break;
    case 0x05: dec_r(gb, R8_B);             break;
    case 0x06: ld_r_n(gb, R8_B);            break;
    case 0x07: rlca(gb);                    break;
    case 0x08: ld_indirect_nn_sp(gb);       break;
    case 0x09: add_hl_rr(gb, R16_BC);       break;
    case 0x0a: ld_a_indirect_rr(gb, R16_BC); break;
    case 0x0b: dec_rr(gb, R16_BC);          break;
    case 0x0c: inc_r(gb, R8_C);             break;
    case 0x0d: dec_r(gb, R8_C);             break;
    case 0x0e: ld_r_n(gb, R8_C);            break;
    case 0x0f: rrca(gb);                    break;
    case 0x10: stop(gb);                    break;
    case 0x11: ld_rr_nn(gb, R16_DE);        break;
    case 0x12: ld_indirect_rr_a(gb, R16_DE); break;
    case 0x13: inc_rr(gb, R16_DE);          break;
    case 0x14: inc_r(gb, R8_D);             break;
    case 0x15: dec_r(gb, R8_D);             break;
    case 0x16: ld_r_n(gb, R8_D);            break;
    case 0x17: rla(gb);                     break;
    case 0x18: jr_i8(gb);                   break;
    case 0x19: add_hl_rr(gb, R16_DE);       break;
    case 0x1a: ld_a_indirect_rr(gb, R16_DE); break;
    case 0x1b: dec_rr(gb, R16_DE);          break;
    case 0x1c: inc_r(gb, R8_E);             break;
    case 0x1d: dec_r(gb, R8_E);             break;
    case 0x1e: ld_r_n(gb, R8_E);            break;
    case 0x1f: rra(gb);                     break;
    case 0x20: jr_f_i8(gb, COND_NZ);        break;
    case 0x21: ld_rr_nn(gb, R16_HL);        break;
    case 0x22: ldi_indirect_hl_a(gb);       break;
    case 0x23: inc_rr(gb, R16_HL);          break;
    case 0x24: inc_r(gb, R8_H);             break;
    case 0x25: dec_r(gb, R8_H);             break;
    case 0x26: ld_r_n(gb, R8_H);            break;
    case 0x27: daa(gb);                     break;
    case 0x28: jr_f_i8(gb, COND_Z);         break;
    case 0x29: add_hl_rr(gb, R16_HL);       break;
    case 0x2a: ldi_a_indirect_hl(gb);       break;
    case 0x2b: dec_rr(gb, R16_HL);          break;
    case 0x2c: inc_r(gb, R8_L);             break;
    case 0x2d: dec_r(gb, R8_L);             break;
    case 0x2e: ld_r_n(gb, R8_L);            break;
    case 0x2f: cpl(gb);                     break;
    case 0x30: jr_f_i8(gb, COND_NC);        break;
    case 0x31: ld_rr_nn(gb, R16_SP);        break;
    case 0x32: ldd_indirect_hl_a(gb);       break;
    case 0x33: inc_rr(gb, R16_SP);          break;
    case 0x34: inc_indirect_hl(gb);         break;
    case 0x35: dec_indirect_hl(gb);         break;
    case 0x36: ld_indirect_hl_n(gb);        break;
    case 0x37: scf(gb);                     break;
    case 0x38: jr_f_i8(gb, COND_C);         break;
    case 0x39: add_hl_rr(gb, R16_SP);       break;
    case 0x3a: ldd_a_indirect_hl(gb);       break;
    case 0x3b: dec_rr(gb, R16_SP);          break;
    case 0x3c: inc_r(gb, R8_A);             break;
    case 0x3d: dec_r(gb, R8_A);             break;
    case 0x3e: ld_r_n(gb, R8_A);            break;
    case 0x3f: ccf(gb);                     break;
    case 0x40: gb->cpu.regs.b = gb->cpu.regs.b; break;
    case 0x41: gb->cpu.regs.b = gb->cpu.regs.c; break;
    case 0x42: gb->cpu.regs.b = gb->cpu.regs.d; break;
    case 0x43: gb->cpu.regs.b = gb->cpu.regs.e; break;
    case 0x44: gb->cpu.regs.b = gb->cpu.regs.h; break;
    case 0x45: gb->cpu.regs.b = gb->cpu.regs.l; break;
    case 0x46: ld_r_indirect_hl(gb, R8_B);      break;
    case 0x47: gb->cpu.regs.b = gb->cpu.regs.a; break;
    case 0x48: gb->cpu.regs.c = gb->cpu.regs.b; break;
    case 0x49: gb->cpu.regs.c = gb->cpu.regs.c; break;
    case 0x4a: gb->cpu.regs.c = gb->cpu.regs.d; break;
    case 0x4b: gb->cpu.regs.c = gb->cpu.regs.e; break;
    case 0x4c: gb->cpu.regs.c = gb->cpu.regs.h; break;
    case 0x4d: gb->cpu.regs.c = gb->cpu.regs.l; break;
    case 0x4e: ld_r_indirect_hl(gb, R8_C);      break;
    case 0x4f: gb->cpu.regs.c = gb->cpu.regs.a; break;
    case 0x50: gb->cpu.regs.d = gb->cpu.regs.b; break;
    case 0x51: gb->cpu.regs.d = gb->cpu.regs.c; break;
    case 0x52: gb->cpu.regs.d = gb->cpu.regs.d; break;
    case 0x53: gb->cpu.regs.d = gb->cpu.regs.e; break;
    case 0x54: gb->cpu.regs.d = gb->cpu.regs.h; break;
    case 0x55: gb->cpu.regs.d = gb->cpu.regs.l; break;
    case 0x56: ld_r_indirect_hl(gb, R8_D);      break;
    case 0x57: gb->cpu.regs.d = gb->cpu.regs.a; break;
    case 0x58: gb->cpu.regs.e = gb->cpu.regs.b; break;
    case 0x59: gb->cpu.regs.e = gb->cpu.regs.c; break;
    case 0x5a: gb->cpu.regs.e = gb->cpu.regs.d; break;
    case 0x5b: gb->cpu.regs.e = gb->cpu.regs.e; break;
    case 0x5c: gb->cpu.regs.e = gb->cpu.regs.h; break;
    case 0x5d: gb->cpu.regs.e = gb->cpu.regs.l; break;
    case 0x5e: ld_r_indirect_hl(gb, R8_E);      break;
    case 0x5f: gb->cpu.regs.e = gb->cpu.regs.a; break;
    case 0x60: gb->cpu.regs.h = gb->cpu.regs.b; break;
    case 0x61: gb->cpu.regs.h = gb->cpu.regs.c; break;
    case 0x62: gb->cpu.regs.h = gb->cpu.regs.d; break;
    case 0x63: gb->cpu.regs.h = gb->cpu.regs.e; break;
    case 0x64: gb->cpu.regs.h = gb->cpu.regs.h; break;
    case 0x65: gb->cpu.regs.h = gb->cpu.regs.l; break;
    case 0x66: ld_r_indirect_hl(gb, R8_H);      break;
    case 0x67: gb->cpu.regs.h = gb->cpu.regs.a; break;
    case 0x68: gb->cpu.regs.l = gb->cpu.regs.b; break;
    case 0x69: gb->cpu.regs.l = gb->cpu.regs.c; break;
    case 0x6a: gb->cpu.regs.l = gb->cpu.regs.d; break;
    case 0x6b: gb->cpu.regs.l = gb->cpu.regs.e; break;
    case 0x6c: gb->cpu.regs.l = gb->cpu.regs.h; break;
    case 0x6d: gb->cpu.regs.l = gb->cpu.regs.l; break;
    case 0x6e: ld_r_indirect_hl(gb, R8_L);      break;
    case 0x6f: gb->cpu.regs.l = gb->cpu.regs.a; break;
    case 0x70: ld_indirect_hl_r(gb, R8_B);        break;
    case 0x71: ld_indirect_hl_r(gb, R8_C);        break;
    case 0x72: ld_indirect_hl_r(gb, R8_D);        break;
    case 0x73: ld_indirect_hl_r(gb, R8_E);        break;
    case 0x74: ld_indirect_hl_r(gb, R8_H);        break;
    case 0x75: ld_indirect_hl_r(gb, R8_L);        break;
    case 0x76: halt(gb);                    break;
    case 0x77: ld_indirect_hl_r(gb, R8_A);        break;
    case 0x78: gb->cpu.regs.a = gb->cpu.regs.b; break;
    case 0x79: gb->cpu.regs.a = gb->cpu.regs.c; break;
    case 0x7a: gb->cpu.regs.a = gb->cpu.regs.d; break;
    case 0x7b: gb->cpu.regs.a = gb->cpu.regs.e; break;
    case 0x7c: gb->cpu.regs.a = gb->cpu.regs.h; break;
    case 0x7d: gb->cpu.regs.a = gb->cpu.regs.l; break;
    case 0x7e: ld_r_indirect_hl(gb, R8_A);      break;
    case 0x7f: gb->cpu.regs.a = gb->cpu.regs.a; break;
    case 0x80: add_a_r(gb, R8_B);               break;
    case 0x81: add_a_r(gb, R8_C);               break;
    case 0x82: add_a_r(gb, R8_D);               break;
    case 0x83: add_a_r(gb, R8_E);               break;
    case 0x84: add_a_r(gb, R8_H);               break;
    case 0x85: add_a_r(gb, R8_L);               break;
    case 0x86: add_a_indirect_hl(gb);           break;
    case 0x87: add_a_r(gb, R8_A);               break;
    case 0x88: adc_a_r(gb, R8_B);               break;
    case 0x89: adc_a_r(gb, R8_C);               break;
    case 0x8a: adc_a_r(gb, R8_D);               break;
    case 0x8b: adc_a_r(gb, R8_E);               break;
    case 0x8c: adc_a_r(gb, R8_H);               break;
    case 0x8d: adc_a_r(gb, R8_L);               break;
    case 0x8e: adc_a_indirect_hl(gb);               break;
    case 0x8f: adc_a_r(gb, R8_A);               break;
    case 0x90: sub_a_r(gb, R8_B);               break;
    case 0x91: sub_a_r(gb, R8_C);               break;
    case 0x92: sub_a_r(gb, R8_D);               break;
    case 0x93: sub_a_r(gb, R8_E);               break;
    case 0x94: sub_a_r(gb, R8_H);               break;
    case 0x95: sub_a_r(gb, R8_L);               break;
    case 0x96: sub_a_indirect_hl(gb);           break;
    case 0x97: sub_a_r(gb, R8_A);               break;
    case 0x98: sbc_a_r(gb, R8_B);               break;
    case 0x99: sbc_a_r(gb, R8_C);               break;
    case 0x9a: sbc_a_r(gb, R8_D);               break;
    case 0x9b: sbc_a_r(gb, R8_E);               break;
    case 0x9c: sbc_a_r(gb, R8_H);               break;
    case 0x9d: sbc_a_r(gb, R8_L);               break;
    case 0x9e: sbc_a_indirect_hl(gb);               break;
    case 0x9f: sbc_a_r(gb, R8_A);               break;
    case 0xa0: and_a_r(gb, R8_B);               break;
    case 0xa1: and_a_r(gb, R8_C);               break;
    case 0xa2: and_a_r(gb, R8_D);               break;
    case 0xa3: and_a_r(gb, R8_E);               break;
    case 0xa4: and_a_r(gb, R8_H);               break;
    case 0xa5: and_a_r(gb, R8_L);               break;
    case 0xa6: and_a_indirect_hl(gb);           break;
    case 0xa7: and_a_r(gb, R8_A);               break;
    case 0xa8: xor_a_r(gb, R8_B);               break;
    case 0xa9: xor_a_r(gb, R8_C);               break;
    case 0xaa: xor_a_r(gb, R8_D);               break;
    case 0xab: xor_a_r(gb, R8_E);               break;
    case 0xac: xor_a_r(gb, R8_H);               break;
    case 0xad: xor_a_r(gb, R8_L);               break;
    case 0xae: xor_a_indirect_hl(gb);           break;
    case 0xaf: xor_a_r(gb, R8_A);           break;
    case 0xb0: or_a_r(gb, R8_B);               break;
    case 0xb1: or_a_r(gb, R8_C);               break;
    case 0xb2: or_a_r(gb, R8_D);               break;
    case 0xb3: or_a_r(gb, R8_E);               break;
    case 0xb4: or_a_r(gb, R8_H);               break;
    case 0xb5: or_a_r(gb, R8_L);               break;
    case 0xb6: or_a_indirect_hl(gb);           break;
    case 0xb7: or_a_r(gb, R8_A);               break;
    case 0xb8: cp_a_r(gb, R8_B);               break;
    case 0xb9: cp_a_r(gb, R8_C);               break;
    case 0xba: cp_a_r(gb, R8_D);               break;
    case 0xbb: cp_a_r(gb, R8_E);               break;
    case 0xbc: cp_a_r(gb, R8_H);               break;
    case 0xbd: cp_a_r(gb, R8_L);               break;
    case 0xbe: cp_a_indirect_hl(gb);           break;
    case 0xbf: cp_a_r(gb, R8_A);               break;
    case 0xc0: ret_f(gb, COND_NZ);              break;
    case 0xc1: pop_rr(gb, R16_BC);          break;
    case 0xc2: jp_f_nn(gb, COND_NZ);        break;
    case 0xc3: jp_nn(gb);                   break;
    case 0xc4: call_f_nn(gb, COND_NZ);      break;
    case 0xc5: push_rr(gb, R16_BC);         break;
    case 0xc6: add_a_n(gb);                 break;
    case 0xc7: rst_n(gb, 0x00);             break;
    case 0xc8: ret_f(gb, COND_Z);           break;
    case 0xc9: ret(gb);                     break;
    case 0xca: jp_f_nn(gb, COND_Z);         break;
    case 0xcb: execute_cb_instructions(gb); break;
    case 0xcc: call_f_nn(gb, COND_Z);       break;
    case 0xcd: call_nn(gb);                 break;
    case 0xce: adc_a_n(gb);                 break;
    case 0xcf: rst_n(gb, 0x08);             break;
    case 0xd0: ret_f(gb, COND_NC);          break;
    case 0xd1: pop_rr(gb, R16_DE);          break;
    case 0xd2: jp_f_nn(gb, COND_NC);        break;
    case 0xd3:                              break;
    case 0xd4: call_f_nn(gb, COND_NC);      break;
    case 0xd5: push_rr(gb, R16_DE);         break;
    case 0xd6: sub_a_n(gb);                 break;
    case 0xd7: rst_n(gb, 0x10);             break;
    case 0xd8: ret_f(gb, COND_C);           break;
    case 0xd9: reti(gb);                    break;
    case 0xda: jp_f_nn(gb, COND_C);         break;
    case 0xdb:                              break;
    case 0xdc: call_f_nn(gb, COND_C);       break;
    case 0xdd:                              break;
    case 0xde: sbc_a_n(gb);                 break;
    case 0xdf: rst_n(gb, 0x18);             break;
    case 0xe0: ldh_n_a(gb);                 break;
    case 0xe1: pop_rr(gb, R16_HL);          break;
    case 0xe2: ldh_r_a(gb, R8_C);           break;
    case 0xe3:                              break;
    case 0xe4:                              break;
    case 0xe5: push_rr(gb, R16_HL);         break;
    case 0xe6: and_a_n(gb);                 break;
    case 0xe7: rst_n(gb, 0x20);             break;
    case 0xe8: add_sp_i8(gb);               break; 
    case 0xe9: jp_hl(gb);                   break;
    case 0xea: ld_indirect_nn_a(gb);        break;
    case 0xeb:                              break;
    case 0xec:                              break;
    case 0xed:                              break;
    case 0xee: xor_a_n(gb);                 break;
    case 0xef: rst_n(gb, 0x28);             break;
    case 0xf0: ldh_a_n(gb);                 break;
    case 0xf1: pop_rr(gb, R16_AF);          break;
    case 0xf2: ldh_a_r(gb, R8_C);           break;
    case 0xf3: di(gb);                      break;
    case 0xf4:                              break;
    case 0xf5: push_rr(gb, R16_AF);         break;
    case 0xf6: or_a_n(gb);                  break;
    case 0xf7: rst_n(gb, 0x30);             break;
    case 0xf8: ld_hl_sp_plus_i8(gb);        break;
    case 0xf9: ld_sp_hl(gb);                break;
    case 0xfa: ld_a_indirect_nn(gb);        break;
    case 0xfb: ei(gb);                      break;
    case 0xfc:                              break;
    case 0xfd:                              break;
    case 0xfe: cp_a_n(gb);                  break;
    case 0xff: rst_n(gb, 0x38);             break;
    default:
        printf("Instruction 0x%02x is invalid\n", opcode);
        exit(EXIT_FAILURE);
    }
}

void cpu_step(struct gb *gb)
{
    switch (gb->cpu.mode) {
    case NORMAL:
        execute_normal_instructions(gb);
        break;
    default:
        break;
    }
}

void cpu_init(struct gb *gb)
{
    gb->cpu.mode = NORMAL;
    gb->cpu.regs.pc = 0;
}