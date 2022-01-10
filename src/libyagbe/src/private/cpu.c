/* Copyright 2022 Michael Rodriguez <mike@kaichiuchu.dev>
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH
 * REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT,
 * INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
 * LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
 * OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 */

#include "libyagbe/cpu.h"

#include "libyagbe/bus.h"
#include "libyagbe/compat/compat_stdbool.h"
#include "libyagbe/debug/logger.h"
#include "libyagbe/scheduler.h"
#include "utility.h"

enum cpu_opcodes {
  OP_NOP = 0x00,
  OP_LD_BC_IMM16 = 0x01,
  OP_INC_BC = 0x03,
  OP_INC_B = 0x04,
  OP_DEC_B = 0x05,
  OP_LD_B_IMM8 = 0x06,
  OP_DEC_C = 0x0D,
  OP_LD_C_IMM8 = 0x0E,
  OP_LD_DE_IMM16 = 0x11,
  OP_LD_MEM_DE_A = 0x12,
  OP_INC_DE = 0x13,
  OP_INC_D = 0x14,
  OP_JR_SIMM8 = 0x18,
  OP_LD_A_MEM_DE = 0x1A,
  OP_INC_E = 0x1C,
  OP_DEC_E = 0x1D,
  OP_RRA = 0x1F,
  OP_JR_NZ_SIMM8 = 0x20,
  OP_LD_HL_IMM16 = 0x21,
  OP_LDI_MEM_HL_A = 0x22,
  OP_INC_HL = 0x23,
  OP_INC_H = 0x24,
  OP_DEC_H = 0x25,
  OP_LD_H_IMM8 = 0x26,
  OP_JR_Z_SIMM8 = 0x28,
  OP_ADD_HL_HL = 0x29,
  OP_LDI_A_MEM_HL = 0x2A,
  OP_INC_L = 0x2C,
  OP_DEC_L = 0x2D,
  OP_JR_NC_SIMM8 = 0x30,
  OP_LD_SP_IMM16 = 0x31,
  OP_LDD_MEM_HL_A = 0x32,
  OP_DEC_MEM_HL = 0x35,
  OP_JR_C_SIMM8 = 0x38,
  OP_DEC_A = 0x3D,
  OP_LD_A_IMM8 = 0x3E,
  OP_LD_B_MEM_HL = 0x46,
  OP_LD_B_A = 0x47,
  OP_LD_C_MEM_HL = 0x4E,
  OP_LD_C_A = 0x4F,
  OP_LD_D_MEM_HL = 0x56,
  OP_LD_D_A = 0x57,
  OP_LD_E_A = 0x5F,
  OP_LD_H_A = 0x67,
  OP_LD_L_MEM_HL = 0x6E,
  OP_LD_L_A = 0x6F,
  OP_LD_MEM_HL_B = 0x70,
  OP_LD_MEM_HL_C = 0x71,
  OP_LD_MEM_HL_D = 0x72,
  OP_LD_MEM_HL_A = 0x77,
  OP_LD_A_B = 0x78,
  OP_LD_A_C = 0x79,
  OP_LD_A_D = 0x7A,
  OP_LD_A_E = 0x7B,
  OP_LD_A_H = 0x7C,
  OP_LD_A_L = 0x7D,
  OP_LD_A_MEM_HL = 0x7E,
  OP_ADD_A_C = 0x81,
  OP_SUB_C = 0x91,
  OP_XOR_C = 0xA9,
  OP_XOR_MEM_HL = 0xAE,
  OP_OR_C = 0xB1,
  OP_OR_MEM_HL = 0xB6,
  OP_OR_A = 0xB7,
  OP_CP_C = 0xB9,
  OP_POP_BC = 0xC1,
  OP_JP_NZ_IMM16 = 0xC2,
  OP_JP_IMM16 = 0xC3,
  OP_CALL_NZ_IMM16 = 0xC4,
  OP_PUSH_BC = 0xC5,
  OP_ADD_A_IMM8 = 0xC6,
  OP_RET_Z = 0xC8,
  OP_RET = 0xC9,
  OP_JP_Z_IMM16 = 0xCA,
  OP_PREFIX_CB = 0xCB,
  OP_CALL_IMM16 = 0xCD,
  OP_ADC_A_IMM8 = 0xCE,
  OP_RET_NC = 0xD0,
  OP_POP_DE = 0xD1,
  OP_CALL_NC_IMM16 = 0xD4,
  OP_PUSH_DE = 0xD5,
  OP_SUB_IMM8 = 0xD6,
  OP_RET_C = 0xD8,
  OP_LDH_IMM8_A = 0xE0,
  OP_POP_HL = 0xE1,
  OP_PUSH_HL = 0xE5,
  OP_AND_IMM8 = 0xE6,
  OP_JP_HL = 0xE9,
  OP_LD_MEM_IMM16_A = 0xEA,
  OP_XOR_IMM8 = 0xEE,
  OP_LDH_A_IMM8 = 0xF0,
  OP_POP_AF = 0xF1,
  OP_DI = 0xF3,
  OP_PUSH_AF = 0xF5,
  OP_LD_A_MEM_IMM16 = 0xFA,
  OP_CP_IMM8 = 0xFE
};

enum cb_opcodes { OP_RR_C = 0x19, OP_RR_D = 0x1A, OP_SRL_B = 0x38 };

enum cpu_flags { FLAG_Z = 7, FLAG_C = 4 };

enum alu_flag {
  ALU_NORMAL,
  ALU_WITH_CARRY,
  ALU_CLEAR_ZERO,
  ALU_DISCARD_RESULT
};

static struct libyagbe_cpu cpu;

static uint8_t read_imm8(void) {
  return libyagbe_bus_read_memory(cpu.reg.pc.value++);
}

static void set_zero_flag(const uint8_t value) {
  SET_BIT_IF(cpu.reg.af.byte.lo, FLAG_Z, value == 0);
}

static void set_carry_flag(const bool condition_met) {
  SET_BIT_IF(cpu.reg.af.byte.lo, FLAG_C, condition_met);
}

static bool zero_flag_is_set(void) {
  return BIT_IS_SET(cpu.reg.af.byte.lo, FLAG_Z);
}

static bool carry_flag_is_set(void) {
  return BIT_IS_SET(cpu.reg.af.byte.lo, FLAG_C);
}

static uint8_t alu_inc(uint8_t value) {
  value++;

  set_zero_flag(value);
  return value;
}

static uint8_t alu_dec(uint8_t value) {
  value--;

  set_zero_flag(value);
  return value;
}

static void alu_add(const int addend) {
  int sum;
  uint8_t result;

  sum = cpu.reg.af.byte.hi + addend;

  result = (uint8_t)sum;

  set_zero_flag(result);
  set_carry_flag(sum > 0xFF);

  cpu.reg.af.byte.hi = result;
}

static void alu_adc(const uint8_t addend) {
  const uint8_t carry_flag_value = carry_flag_is_set();
  alu_add(addend + carry_flag_value);
}

static void alu_add_hl(const uint16_t pair) {
  int sum;

  sum = cpu.reg.hl.value + pair;
  set_carry_flag(sum > 0xFFFF);

  cpu.reg.hl.value = (uint16_t)sum;
}

static void alu_sub(const uint8_t subtrahend, const enum alu_flag flag) {
  int diff;
  uint8_t result;

  diff = cpu.reg.af.byte.hi - subtrahend;
  result = (uint8_t)diff;

  set_zero_flag(result);
  set_carry_flag(diff < 0);

  if (flag != ALU_DISCARD_RESULT) {
    cpu.reg.af.byte.hi = result;
  }
}

static uint8_t alu_srl(uint8_t reg) {
  set_carry_flag(reg & 1);
  reg >>= 1;
  set_zero_flag(reg);

  return reg;
}

static uint8_t alu_rr(uint8_t reg, const enum alu_flag flag) {
  uint8_t old_carry_flag_value;

  old_carry_flag_value = carry_flag_is_set() ? 0x80 : 0x00;

  set_carry_flag(reg & 1);

  reg >>= 1;
  reg |= old_carry_flag_value;

  if (flag == ALU_CLEAR_ZERO) {
    cpu.reg.af.byte.lo &= ~(1 << FLAG_Z);
    return reg;
  }

  set_zero_flag(reg);
  return reg;
}

static uint16_t read_imm16(void) {
  uint8_t lo;
  uint8_t hi;

  lo = read_imm8();
  hi = read_imm8();

  return (hi << 8) | lo;
}

static void jr_if(const bool condition_met) {
  int8_t imm;

  imm = (int8_t)read_imm8();

  if (condition_met) {
    cpu.reg.pc.value += imm;
    libyagbe_scheduler_add_cycles(12);
  } else {
    libyagbe_scheduler_add_cycles(8);
  }
}

static void stack_push(const uint8_t hi, const uint8_t lo) {
  libyagbe_bus_write_memory(--cpu.reg.sp.value, hi);
  libyagbe_bus_write_memory(--cpu.reg.sp.value, lo);
}

static uint16_t stack_pop(void) {
  uint8_t lo;
  uint8_t hi;

  lo = libyagbe_bus_read_memory(cpu.reg.sp.value++);
  hi = libyagbe_bus_read_memory(cpu.reg.sp.value++);

  return (hi << 8) | lo;
}

static void ret_if(const bool condition_met) {
  if (condition_met) {
    cpu.reg.pc.value = stack_pop();
  }
}

static void jp_if(const bool condition_met) {
  uint16_t address;

  address = read_imm16();

  if (condition_met) {
    cpu.reg.pc.value = address;
  }
}

static void call_if(const bool condition_met) {
  uint16_t address;

  address = read_imm16();

  if (condition_met) {
    stack_push(cpu.reg.pc.byte.hi, cpu.reg.pc.byte.lo);
    cpu.reg.pc.value = address;
  }
}

struct libyagbe_cpu* libyagbe_cpu_get_data(void) {
  return &cpu;
}

void libyagbe_cpu_reset(void) {
  cpu.reg.bc.value = 0x0013;
  cpu.reg.de.value = 0x00D8;
  cpu.reg.hl.value = 0x014D;
  cpu.reg.af.value = 0x01B0;
  cpu.reg.sp.value = 0xFFFE;
  cpu.reg.pc.value = 0x0100;

  cpu.instruction = 0x00;
}

void libyagbe_cpu_step(void) {
  cpu.instruction = read_imm8();

  switch (cpu.instruction) {
    case OP_NOP:
      libyagbe_scheduler_add_cycles(4);
      return;

    case OP_LD_BC_IMM16:
      cpu.reg.bc.value = read_imm16();
      libyagbe_scheduler_add_cycles(12);

      return;

    case OP_INC_BC:
      cpu.reg.bc.value++;
      libyagbe_scheduler_add_cycles(8);

      return;

    case OP_INC_B:
      cpu.reg.bc.byte.hi = alu_inc(cpu.reg.bc.byte.hi);
      libyagbe_scheduler_add_cycles(4);

      return;

    case OP_DEC_B:
      cpu.reg.bc.byte.hi = alu_dec(cpu.reg.bc.byte.hi);
      libyagbe_scheduler_add_cycles(4);

      return;

    case OP_LD_B_IMM8:
      cpu.reg.bc.byte.hi = read_imm8();
      libyagbe_scheduler_add_cycles(8);

      return;

    case OP_DEC_C:
      cpu.reg.bc.byte.lo = alu_dec(cpu.reg.bc.byte.lo);
      libyagbe_scheduler_add_cycles(4);

      return;

    case OP_LD_C_IMM8:
      cpu.reg.bc.byte.lo = read_imm8();
      libyagbe_scheduler_add_cycles(8);

      return;

    case OP_LD_DE_IMM16:
      cpu.reg.de.value = read_imm16();
      libyagbe_scheduler_add_cycles(12);

      return;

    case OP_LD_MEM_DE_A:
      libyagbe_bus_write_memory(cpu.reg.de.value, cpu.reg.af.byte.hi);
      libyagbe_scheduler_add_cycles(8);

      return;

    case OP_INC_DE:
      cpu.reg.de.value++;
      libyagbe_scheduler_add_cycles(8);

      return;

    case OP_INC_D:
      cpu.reg.de.byte.hi = alu_inc(cpu.reg.de.byte.hi);
      libyagbe_scheduler_add_cycles(4);

      return;

    case OP_JR_SIMM8:
      cpu.reg.pc.value += (int8_t)read_imm8();
      libyagbe_scheduler_add_cycles(12);

      return;

    case OP_LD_A_MEM_DE:
      cpu.reg.af.byte.hi = libyagbe_bus_read_memory(cpu.reg.de.value);
      libyagbe_scheduler_add_cycles(8);

      return;

    case OP_INC_E:
      cpu.reg.de.byte.lo = alu_inc(cpu.reg.de.byte.lo);
      libyagbe_scheduler_add_cycles(4);

      return;

    case OP_DEC_E:
      cpu.reg.de.byte.lo = alu_dec(cpu.reg.de.byte.lo);
      libyagbe_scheduler_add_cycles(4);

      return;

    case OP_RRA:
      cpu.reg.af.byte.hi = alu_rr(cpu.reg.af.byte.hi, ALU_CLEAR_ZERO);
      libyagbe_scheduler_add_cycles(4);

      return;

    case OP_JR_NZ_SIMM8:
      jr_if(!zero_flag_is_set());
      return;

    case OP_LD_HL_IMM16:
      cpu.reg.hl.value = read_imm16();
      libyagbe_scheduler_add_cycles(12);

      return;

    case OP_LDI_MEM_HL_A:
      libyagbe_bus_write_memory(cpu.reg.hl.value++, cpu.reg.af.byte.hi);
      libyagbe_scheduler_add_cycles(8);

      return;

    case OP_INC_HL:
      cpu.reg.hl.value++;
      libyagbe_scheduler_add_cycles(8);

      return;

    case OP_INC_H:
      cpu.reg.hl.byte.hi = alu_inc(cpu.reg.hl.byte.hi);
      libyagbe_scheduler_add_cycles(4);

      return;

    case OP_DEC_H:
      cpu.reg.hl.byte.hi = alu_dec(cpu.reg.hl.byte.hi);
      libyagbe_scheduler_add_cycles(4);

      return;

    case OP_LD_H_IMM8:
      cpu.reg.hl.byte.hi = read_imm8();
      libyagbe_scheduler_add_cycles(8);

      return;

    case OP_JR_Z_SIMM8:
      jr_if(zero_flag_is_set());
      return;

    case OP_ADD_HL_HL:
      alu_add_hl(cpu.reg.hl.value);
      libyagbe_scheduler_add_cycles(8);

      return;

    case OP_LDI_A_MEM_HL:
      cpu.reg.af.byte.hi = libyagbe_bus_read_memory(cpu.reg.hl.value++);
      libyagbe_scheduler_add_cycles(8);

      return;

    case OP_INC_L:
      cpu.reg.hl.byte.lo = alu_inc(cpu.reg.hl.byte.lo);
      libyagbe_scheduler_add_cycles(4);

      return;

    case OP_DEC_L:
      cpu.reg.hl.byte.lo = alu_dec(cpu.reg.hl.byte.lo);
      libyagbe_scheduler_add_cycles(4);

      return;

    case OP_JR_NC_SIMM8:
      jr_if(!carry_flag_is_set());
      return;

    case OP_LD_SP_IMM16:
      cpu.reg.sp.value = read_imm16();
      libyagbe_scheduler_add_cycles(12);

      return;

    case OP_LDD_MEM_HL_A:
      libyagbe_bus_write_memory(cpu.reg.hl.value--, cpu.reg.af.byte.hi);
      libyagbe_scheduler_add_cycles(8);

      return;

    case OP_DEC_MEM_HL: {
      uint8_t data = libyagbe_bus_read_memory(cpu.reg.hl.value);
      data = alu_dec(data);
      libyagbe_bus_write_memory(cpu.reg.hl.value, data);

      libyagbe_scheduler_add_cycles(12);
      return;
    }

    case OP_JR_C_SIMM8:
      jr_if(carry_flag_is_set());
      return;

    case OP_DEC_A:
      cpu.reg.af.byte.hi = alu_dec(cpu.reg.af.byte.hi);
      libyagbe_scheduler_add_cycles(4);

      return;

    case OP_LD_A_IMM8:
      cpu.reg.af.byte.hi = read_imm8();
      libyagbe_scheduler_add_cycles(8);

      return;

    case OP_LD_B_MEM_HL:
      cpu.reg.bc.byte.hi = libyagbe_bus_read_memory(cpu.reg.hl.value);
      libyagbe_scheduler_add_cycles(8);

      return;

    case OP_LD_B_A:
      cpu.reg.bc.byte.hi = cpu.reg.af.byte.hi;
      libyagbe_scheduler_add_cycles(4);

      return;

    case OP_LD_C_MEM_HL:
      cpu.reg.bc.byte.lo = libyagbe_bus_read_memory(cpu.reg.hl.value);
      libyagbe_scheduler_add_cycles(8);

      return;

    case OP_LD_C_A:
      cpu.reg.bc.byte.lo = cpu.reg.af.byte.hi;
      libyagbe_scheduler_add_cycles(4);

      return;

    case OP_LD_D_MEM_HL:
      cpu.reg.de.byte.hi = libyagbe_bus_read_memory(cpu.reg.hl.value);
      libyagbe_scheduler_add_cycles(8);

      return;

    case OP_LD_D_A:
      cpu.reg.de.byte.hi = cpu.reg.af.byte.hi;
      libyagbe_scheduler_add_cycles(4);

      return;

    case OP_LD_E_A:
      cpu.reg.de.byte.lo = cpu.reg.af.byte.hi;
      libyagbe_scheduler_add_cycles(4);

      return;

    case OP_LD_H_A:
      cpu.reg.hl.byte.hi = cpu.reg.af.byte.hi;
      libyagbe_scheduler_add_cycles(4);

      return;

    case OP_LD_L_MEM_HL:
      cpu.reg.hl.byte.lo = libyagbe_bus_read_memory(cpu.reg.hl.value);
      libyagbe_scheduler_add_cycles(8);

      return;

    case OP_LD_L_A:
      cpu.reg.hl.byte.lo = cpu.reg.af.byte.hi;
      libyagbe_scheduler_add_cycles(4);

      return;

    case OP_LD_MEM_HL_B:
      libyagbe_bus_write_memory(cpu.reg.hl.value, cpu.reg.bc.byte.hi);
      libyagbe_scheduler_add_cycles(8);

      return;

    case OP_LD_MEM_HL_C:
      libyagbe_bus_write_memory(cpu.reg.hl.value, cpu.reg.bc.byte.lo);
      libyagbe_scheduler_add_cycles(8);

      return;

    case OP_LD_MEM_HL_D:
      libyagbe_bus_write_memory(cpu.reg.hl.value, cpu.reg.de.byte.hi);
      libyagbe_scheduler_add_cycles(8);

      return;

    case OP_LD_MEM_HL_A:
      libyagbe_bus_write_memory(cpu.reg.hl.value, cpu.reg.af.byte.hi);
      libyagbe_scheduler_add_cycles(8);

      return;

    case OP_LD_A_B:
      cpu.reg.af.byte.hi = cpu.reg.bc.byte.hi;
      libyagbe_scheduler_add_cycles(4);

      return;

    case OP_LD_A_C:
      cpu.reg.af.byte.hi = cpu.reg.bc.byte.lo;
      libyagbe_scheduler_add_cycles(4);

      return;

    case OP_LD_A_D:
      cpu.reg.af.byte.hi = cpu.reg.de.byte.hi;
      libyagbe_scheduler_add_cycles(4);

      return;

    case OP_LD_A_E:
      cpu.reg.af.byte.hi = cpu.reg.de.byte.lo;
      libyagbe_scheduler_add_cycles(4);

      return;

    case OP_LD_A_H:
      cpu.reg.af.byte.hi = cpu.reg.hl.byte.hi;
      libyagbe_scheduler_add_cycles(4);

      return;

    case OP_LD_A_L:
      cpu.reg.af.byte.hi = cpu.reg.hl.byte.lo;
      libyagbe_scheduler_add_cycles(4);

      return;

    case OP_LD_A_MEM_HL:
      cpu.reg.af.byte.hi = libyagbe_bus_read_memory(cpu.reg.hl.value);
      libyagbe_scheduler_add_cycles(8);

      return;

    case OP_ADD_A_C:
      alu_add(cpu.reg.bc.byte.lo);
      libyagbe_scheduler_add_cycles(4);

      return;

    case OP_SUB_C:
      alu_sub(cpu.reg.bc.byte.lo, ALU_NORMAL);
      libyagbe_scheduler_add_cycles(4);

      return;

    case OP_XOR_C:
      cpu.reg.af.byte.hi ^= cpu.reg.bc.byte.lo;
      cpu.reg.af.byte.lo = (cpu.reg.af.byte.hi == 0) ? 0x80 : 0x00;

      libyagbe_scheduler_add_cycles(4);
      return;

    case OP_XOR_MEM_HL: {
      const uint8_t data = libyagbe_bus_read_memory(cpu.reg.hl.value);

      cpu.reg.af.byte.hi ^= data;
      cpu.reg.af.byte.lo = (cpu.reg.af.byte.hi == 0) ? 0x80 : 0x00;

      libyagbe_scheduler_add_cycles(8);
      return;
    }

    case OP_OR_C:
      cpu.reg.af.byte.hi |= cpu.reg.bc.byte.lo;
      cpu.reg.af.byte.lo = (cpu.reg.af.byte.hi == 0) ? 0x80 : 0x00;

      libyagbe_scheduler_add_cycles(4);
      return;

    case OP_OR_MEM_HL: {
      const uint8_t data = libyagbe_bus_read_memory(cpu.reg.hl.value);

      cpu.reg.af.byte.hi |= data;
      cpu.reg.af.byte.lo = (cpu.reg.af.byte.hi == 0) ? 0x80 : 0x00;

      libyagbe_scheduler_add_cycles(8);
      return;
    }

    case OP_OR_A:
      cpu.reg.af.byte.lo = (cpu.reg.af.byte.hi == 0) ? 0x80 : 0x00;
      libyagbe_scheduler_add_cycles(4);

      return;

    case OP_CP_C:
      alu_sub(cpu.reg.bc.byte.lo, ALU_DISCARD_RESULT);
      libyagbe_scheduler_add_cycles(4);

      return;

    case OP_POP_BC:
      cpu.reg.bc.value = stack_pop();
      libyagbe_scheduler_add_cycles(12);

      return;

    case OP_JP_NZ_IMM16:
      jp_if(!zero_flag_is_set());
      return;

    case OP_JP_IMM16:
      cpu.reg.pc.value = read_imm16();
      libyagbe_scheduler_add_cycles(16);

      return;

    case OP_CALL_NZ_IMM16:
      call_if(!zero_flag_is_set());
      return;

    case OP_PUSH_BC:
      stack_push(cpu.reg.bc.byte.hi, cpu.reg.bc.byte.lo);
      libyagbe_scheduler_add_cycles(16);

      return;

    case OP_ADD_A_IMM8: {
      const uint8_t imm8 = read_imm8();

      alu_add(imm8);
      libyagbe_scheduler_add_cycles(8);

      return;
    }

    case OP_RET_Z:
      ret_if(zero_flag_is_set());
      return;

    case OP_RET:
      cpu.reg.pc.value = stack_pop();
      libyagbe_scheduler_add_cycles(16);

      return;

    case OP_JP_Z_IMM16:
      jp_if(zero_flag_is_set());
      return;

    case OP_PREFIX_CB: {
      const uint8_t cb_instruction = read_imm8();

      switch (cb_instruction) {
        case OP_RR_C:
          cpu.reg.bc.byte.lo = alu_rr(cpu.reg.bc.byte.lo, ALU_NORMAL);
          libyagbe_scheduler_add_cycles(8);

          return;

        case OP_RR_D:
          cpu.reg.de.byte.hi = alu_rr(cpu.reg.de.byte.hi, ALU_NORMAL);
          libyagbe_scheduler_add_cycles(8);

          return;

        case OP_SRL_B:
          cpu.reg.bc.byte.hi = alu_srl(cpu.reg.bc.byte.hi);
          libyagbe_scheduler_add_cycles(8);

          return;

        default:
          libyagbe_log(
              LIBYAGBE_LOG_LEVEL_CRITICAL,
              "Invalid CB instruction $%02X reached at program counter $%04X.",
              cb_instruction, cpu.reg.pc.value);
          return;
      }
    }

    case OP_CALL_IMM16: {
      const uint16_t address = read_imm16();

      stack_push(cpu.reg.pc.byte.hi, cpu.reg.pc.byte.lo);
      cpu.reg.pc.value = address;

      libyagbe_scheduler_add_cycles(24);
      return;
    }

    case OP_ADC_A_IMM8: {
      const uint8_t imm8 = read_imm8();

      alu_adc(imm8);
      libyagbe_scheduler_add_cycles(8);

      return;
    }

    case OP_RET_NC:
      ret_if(!carry_flag_is_set());
      return;

    case OP_POP_DE:
      cpu.reg.de.value = stack_pop();
      libyagbe_scheduler_add_cycles(12);

      return;

    case OP_CALL_NC_IMM16:
      call_if(!carry_flag_is_set());
      return;

    case OP_PUSH_DE:
      stack_push(cpu.reg.de.byte.hi, cpu.reg.de.byte.lo);
      libyagbe_scheduler_add_cycles(16);

      return;

    case OP_SUB_IMM8: {
      const uint8_t imm8 = read_imm8();
      alu_sub(imm8, ALU_NORMAL);

      libyagbe_scheduler_add_cycles(8);
      return;
    }

    case OP_RET_C:
      ret_if(carry_flag_is_set());
      return;

    case OP_LDH_IMM8_A: {
      const uint8_t imm8 = read_imm8();

      libyagbe_bus_write_memory(0xFF00 + imm8, cpu.reg.af.byte.hi);
      libyagbe_scheduler_add_cycles(12);

      return;
    }

    case OP_POP_HL:
      cpu.reg.hl.value = stack_pop();
      libyagbe_scheduler_add_cycles(12);

      return;

    case OP_PUSH_HL:
      stack_push(cpu.reg.hl.byte.hi, cpu.reg.hl.byte.lo);
      libyagbe_scheduler_add_cycles(16);

      return;

    case OP_AND_IMM8: {
      const uint8_t imm8 = read_imm8();

      cpu.reg.af.byte.hi &= imm8;
      cpu.reg.af.byte.lo = (cpu.reg.af.byte.hi == 0) ? 0xA0 : 0x20;

      return;
    }

    case OP_JP_HL:
      cpu.reg.pc.value = cpu.reg.hl.value;
      libyagbe_scheduler_add_cycles(4);

      return;

    case OP_LD_MEM_IMM16_A: {
      const uint16_t address = read_imm16();

      libyagbe_bus_write_memory(address, cpu.reg.af.byte.hi);
      return;
    }

    case OP_XOR_IMM8: {
      const uint8_t imm8 = read_imm8();

      cpu.reg.af.byte.hi ^= imm8;
      cpu.reg.af.byte.lo = (cpu.reg.af.byte.hi == 0) ? 0x80 : 0x00;

      return;
    }

    case OP_LDH_A_IMM8: {
      const uint8_t imm8 = read_imm8();

      cpu.reg.af.byte.hi = libyagbe_bus_read_memory(0xFF00 + imm8);
      return;
    }

    case OP_POP_AF:
      cpu.reg.af.value = stack_pop();
      cpu.reg.af.byte.lo &= ~0x0F;

      return;

    /* Interrupts are not supported yet, this is a NOP as such. */
    case OP_DI:
      return;

    case OP_PUSH_AF:
      stack_push(cpu.reg.af.byte.hi, cpu.reg.af.byte.lo);
      return;

    case OP_LD_A_MEM_IMM16: {
      const uint16_t address = read_imm16();

      cpu.reg.af.byte.hi = libyagbe_bus_read_memory(address);
      return;
    }

    case OP_CP_IMM8: {
      const uint8_t imm8 = read_imm8();

      alu_sub(imm8, ALU_DISCARD_RESULT);
      return;
    }

    default:
      break;
  }

  libyagbe_log(LIBYAGBE_LOG_LEVEL_CRITICAL,
               "Invalid instruction $%02X reached at program counter $%04X.",
               cpu.instruction, cpu.reg.pc.value);
}