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

#ifndef LIBYAGBE_LOGGER_H
#define LIBYAGBE_LOGGER_H

#include <stdarg.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef void (*libyagbe_log_cb)(const char* const str);

enum libyagbe_log_level {
  LIBYAGBE_LOG_LEVEL_INFO,
  LIBYAGBE_LOG_LEVEL_WARNING,
  LIBYAGBE_LOG_LEVEL_CRITICAL
};

struct libyagbe_logger {
  libyagbe_log_cb info_cb;
  libyagbe_log_cb warning_cb;
  libyagbe_log_cb critical_cb;
};

void libyagbe_logger_set_log_level_cb(const enum libyagbe_log_level log_level,
                                      const libyagbe_log_cb cb_func);

void libyagbe_log(const enum libyagbe_log_level log_level,
                  const char* const str, ...);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LIBYAGBE_LOGGER_H */