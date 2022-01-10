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

#ifndef LIBYAGBE_PPU_H
#define LIBYAGBE_PPU_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

enum libyagbe_ppu_io_regs {
  LIBYAGBE_PPU_IO_REG_LCDC = 0x0,
  LIBYAGBE_PPU_IO_REG_SCY = 0x2,
  LIBYAGBE_PPU_IO_REG_SCX = 0x3,
  LIBYAGBE_PPU_IO_REG_LY = 0x4,
  LIBYAGBE_PPU_IO_REG_BGP = 0x7
};

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LIBYAGBE_PPU_H */