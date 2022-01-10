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

#ifndef LIBYAGBE_TIMER_H
#define LIBYAGBE_TIMER_H

#include "compat/compat_stdint.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @brief Defines the structure of the Game Boy timer.
 *
 */
struct libyagbe_timer {
  uint8_t tima;
  uint8_t tma;
  uint8_t tac;
};

/**
 * @brief Defines the memory addresses of each timer register.
 *
 */
enum libyagbe_timer_io_regs {
  /** $FF05 */
  LIBYAGBE_TIMER_IO_REG_TIMA = 0x5,

  /** $FF06 */
  LIBYAGBE_TIMER_IO_REG_TMA = 0x6,

  /** $FF07 */
  LIBYAGBE_TIMER_IO_REG_TAC = 0x7
};

/**
 * @brief Defines the bits and masks for the TAC register.
 *
 */
enum libyagbe_timer_tac_bits {
  LIBYAGBE_TIMER_TAC_CLOCK_MASK = 0x03,
  LIBYAGBE_TIMER_TAC_ENABLED = 2
};

/**
 * @brief Resets the timer to the startup state.
 *
 */
void libyagbe_timer_reset(void);
void libyagbe_timer_handle_tima_write(const uint8_t new_tima_value);
void libyagbe_timer_handle_tma_write(const uint8_t new_tma_value);
void libyagbe_timer_handle_tac_write(const uint8_t new_tac_value);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LIBYAGBE_TIMER_H */