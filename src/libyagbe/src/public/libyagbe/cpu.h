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

#ifndef LIBYAGBE_CPU_H
#define LIBYAGBE_CPU_H

#include "compat/compat_stdint.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* Forward declaration. */
struct libyagbe_bus;

/* XXX: On older compilers, though I seriously doubt it, this may be dangerous.
 *  Need to investigate.
 *
 *  Maybe we can come up with some sort of abstraction layer, e.g. use anonymous
 *  structs/unions if the standard is >=C11 or else fall back to this? How would
 *  we handle accesses and declarations cleanly?
 */
typedef union _libyagbe_cpu_register_pair {
  uint16_t value;

  struct {
#ifdef LIBYAGBE_BIG_ENDIAN
    uint8_t hi;
    uint8_t lo;
#else
    uint8_t lo;
    uint8_t hi;
#endif /* LIBYAGBE_BIG_ENDIAN */
  } byte;
} libyagbe_cpu_register_pair;

/**
 * @brief Defines the structure of an SM83 CPU.
 *
 */
struct libyagbe_cpu {
  struct libyagbe_cpu_registers {
    libyagbe_cpu_register_pair af;
    libyagbe_cpu_register_pair bc;
    libyagbe_cpu_register_pair de;
    libyagbe_cpu_register_pair hl;
    libyagbe_cpu_register_pair pc;
    libyagbe_cpu_register_pair sp;
  } reg;

  /** The current instruction being processed. */
  uint8_t instruction;
};

/**
 * @brief Resets the SM83 CPU to the startup state.
 *
 * The startup state is defined as the register values which are set when the
 * boot ROM has completed with interrupts disabled.
 *
 * This function should not be called directly; use \ref libyagbe_system_reset()
 * instead.
 */
void libyagbe_cpu_reset(void);

/**
 * @brief Executes the next instruction.
 *
 * If an invalid instruction was reached, invalid callback...
 *
 * This function should not be called directly; use \ref libyagbe_system_step()
 * instead.
 */
void libyagbe_cpu_step(void);

/**
 * @brief Returns the current SM83 CPU instance.
 *
 * @return struct libyagbe_cpu*
 */
struct libyagbe_cpu* libyagbe_cpu_get_data(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LIBYAGBE_CPU_H */