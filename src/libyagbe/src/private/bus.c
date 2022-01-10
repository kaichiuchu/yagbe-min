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

#include "libyagbe/bus.h"

#include <stdio.h>

#include "libyagbe/apu.h"
#include "libyagbe/debug/logger.h"
#include "libyagbe/ppu.h"
#include "libyagbe/scheduler.h"
#include "libyagbe/timer.h"
#include "utility.h"

static struct libyagbe_bus bus;
static uint8_t* cart_data;

struct libyagbe_bus* libyagbe_bus_get_data(void) {
  return &bus;
}

void libyagbe_bus_set_interrupt(const enum libyagbe_bus_if_bits interrupt) {
  SET_BIT(bus.interrupt_flag, interrupt);
}

void libyagbe_bus_set_cart_data(uint8_t* const data) { cart_data = data; }

uint8_t libyagbe_bus_read_memory(const uint16_t address) {
  switch (address >> 12) {
    case 0x0:
    case 0x4:
      return cart_data[address];

    case 0xC:
      return bus.wram0[address - 0xC000];

    case 0xD:
      return bus.wram1[address - 0xD000];

    case 0xF:
      switch ((address >> 8) & 0x0F) {
        case 0xF:
          switch ((address & 0x00FF) >> 4) {
            case 0x0:
              switch (address & 0x000F) {
                case LIBYAGBE_BUS_IO_REG_IF:
                  return bus.interrupt_flag;

                default:
                  break;
              }
              break;

            case 0x4:
              switch (address & 0x000F) {
                /* Stubbed. */
                case LIBYAGBE_PPU_IO_REG_LY:
                  return 0xFF;

                default:
                  break;
              }
              break;

            case 0x8:
              return bus.hram[address - 0xFF80];

            case 0xF:
              switch (address & 0x000F) {
                case 0xF:
                  return bus.interrupt_enable;

                default:
                  break;
              }
              break;
          }
          break;

        default:
          break;
      }
      break;

    default:
      break;
  }

  libyagbe_log(LIBYAGBE_LOG_LEVEL_WARNING,
               "Unhandled memory read: $%04X, returning $FF", address);
  return 0xFF;
}

void libyagbe_bus_write_memory(const uint16_t address, const uint8_t data) {
  switch (address >> 12) {
    /* Stubbed. */
    case 0x8:
    case 0x9:
      return;

    case 0xC:
      bus.wram0[address - 0xC000] = data;
      return;

    case 0xD:
      bus.wram1[address - 0xD000] = data;
      return;

    case 0xF:
      switch ((address >> 8) & 0x0F) {
        case 0xF:
          switch ((address & 0x00FF) >> 4) {
            case 0x0:
              switch (address & 0x000F) {
                case LIBYAGBE_BUS_IO_REG_SB:
                  putchar(data);
                  return;

                /* Stubbed. */
                case LIBYAGBE_BUS_IO_REG_SC:
                  return;

                case LIBYAGBE_TIMER_IO_REG_TIMA:
                  libyagbe_timer_handle_tima_write(data);
                  return;

                case LIBYAGBE_TIMER_IO_REG_TMA:
                  libyagbe_timer_handle_tma_write(data);
                  return;

                case LIBYAGBE_TIMER_IO_REG_TAC:
                  libyagbe_timer_handle_tac_write(data);
                  return;

                case LIBYAGBE_BUS_IO_REG_IF:
                  bus.interrupt_flag = data;
                  return;

                default:
                  break;
              }
              break;

            case 0x2:
              switch (address & 0x000F) {
                /* Stubbed. */
                case LIBYAGBE_APU_IO_REG_NR50:
                case LIBYAGBE_APU_IO_REG_NR51:
                case LIBYAGBE_APU_IO_REG_NR52:
                  return;

                default:
                  break;
              }
              break;

            case 0x4:
              switch (address & 0x000F) {
                /* Stubbed. */
                case LIBYAGBE_PPU_IO_REG_LCDC:
                case LIBYAGBE_PPU_IO_REG_SCY:
                case LIBYAGBE_PPU_IO_REG_SCX:
                case LIBYAGBE_PPU_IO_REG_BGP:
                  return;

                default:
                  break;
              }
              break;

            case 0x8:
              bus.hram[address - 0xFF80] = data;
              return;

            case 0xF:
              switch (address & 0x000F) {
                case 0xF:
                  bus.interrupt_enable = data;
                  return;

                default:
                  break;
              }
              break;
          }
          break;
      }
      break;

    default:
      break;
  }

  libyagbe_log(LIBYAGBE_LOG_LEVEL_WARNING,
               "Unhandled memory write: $%04X <- $%02X", address, data);
}