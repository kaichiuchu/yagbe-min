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

#ifndef LIBYAGBE_APU_H
#define LIBYAGBE_APU_H

#include "compat/compat_stdint.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

enum libyagbe_apu_io_regs {
  LIBYAGBE_APU_IO_REG_NR50 = 0x4,
  LIBYAGBE_APU_IO_REG_NR51 = 0x5,
  LIBYAGBE_APU_IO_REG_NR52 = 0x6
};

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LIBYAGBE_APU_H */