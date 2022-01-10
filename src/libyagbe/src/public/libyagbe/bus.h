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

#ifndef LIBYAGBE_BUS_H
#define LIBYAGBE_BUS_H

#include "compat/compat_stdint.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @brief The sizes of memory areas in bytes.
 */
enum libyagbe_bus_mem_sizes {
  LIBYAGBE_BUS_MEM_SIZE_WRAM = 4096,
  LIBYAGBE_BUS_MEM_SIZE_HRAM = 128
};

/**
 * @brief Defines the bus memory registers.
 */
enum libyagbe_bus_io_regs {
  LIBYAGBE_BUS_IO_REG_SB = 0x1,
  LIBYAGBE_BUS_IO_REG_SC = 0x2,
  LIBYAGBE_BUS_IO_REG_IF = 0xF
};

enum libyagbe_bus_if_bits { LIBYAGBE_BUS_IF_TIMER = 2 };

/** @brief This structure defines the interconnect between the CPU and devices.
 */
struct libyagbe_bus {
  uint8_t wram0[LIBYAGBE_BUS_MEM_SIZE_WRAM];
  uint8_t wram1[LIBYAGBE_BUS_MEM_SIZE_WRAM];

  uint8_t hram[LIBYAGBE_BUS_MEM_SIZE_HRAM];

  uint8_t interrupt_flag;
  uint8_t interrupt_enable;
};

/**
 * @brief Returns the bus structure.
 *
 * This is useful if you want to inspect memory areas, change the values of
 * memory areas, or bus registers.
 *
 * @return struct libyagbe_bus*
 */
struct libyagbe_bus* libyagbe_bus_get_data(void);

/**
 * @brief Enables the specified interrupt.
 *
 * @param interrupt The interrupt to set as triggered.
 */
void libyagbe_bus_set_interrupt(const enum libyagbe_bus_if_bits interrupt);

/**
 * @brief Sets a pointer to the cartridge data to use.
 *
 * The cartridge data will not be copied to an internal buffer, it is your
 * responsibility to make sure that the pointer remains valid.
 *
 * @param data The pointer to the cartridge data.
 */
void libyagbe_bus_set_cart_data(uint8_t* const data);

/**
 * @brief Reads a byte from memory or I/O devices.
 *
 * @param address The memory address to read from.
 * @return uint8_t The value from memory or an I/O device.
 */
uint8_t libyagbe_bus_read_memory(const uint16_t address);

/**
 * @brief Writes a byte to memory or I/O devices.
 *
 * @param address The memory address to write to.
 * @param data The data to write to the memory address.
 */
void libyagbe_bus_write_memory(const uint16_t address, const uint8_t data);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LIBYAGBE_BUS_H */