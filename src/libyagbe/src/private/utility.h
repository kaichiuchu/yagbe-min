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

#ifndef UTILITY_H
#define UTILITY_H

#define BIT_IS_SET(x, b) ((x & (1 << b)) != 0)
#define CLEAR_BIT(x, b) (x &= ~(1 << b))
#define SET_BIT(x, b) (x |= (1 << b))

/* Conditionally sets a bit or clears one without branching. */
#define SET_BIT_IF(n, b, condition) \
  n = (n & ~(1 << b)) | (-(condition) & (1 << b))

#define SWAP(x, y, T) \
  do {                \
    T TEMP = x;       \
    x = y;            \
    y = TEMP;         \
  } while (0)

#endif /* UTILITY_H */