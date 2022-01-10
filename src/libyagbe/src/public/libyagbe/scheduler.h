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

#ifndef LIBYAGBE_SCHEDULER_H
#define LIBYAGBE_SCHEDULER_H

#include <stddef.h>

#include "compat/compat_stdint.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/** @brief The maximum number of events possible. */
#define LIBYAGBE_SCHEDULER_MAX_EVENTS 10

/**
 * @brief The type of events that we support.
 *
 * We need a good way to identify events.
 */
enum libyagbe_scheduler_event_types {
  LIBYAGBE_SCHEDULER_EVENT_TIMA_INCREMENT,
  LIBYAGBE_SCHEDULER_EVENT_TIMA_OVERFLOW
};

enum libyagbe_scheduler_event_groups { LIBYAGBE_SCHEDULER_EVENT_GROUP_TIMER };

typedef void (*libyagbe_scheduler_event_cb)(void);

struct libyagbe_scheduler_event {
  /** How many cycles in the future should this event be called? */
  uintmax_t timestamp;

  /** What function should this event call once it has expired? */
  libyagbe_scheduler_event_cb cb_func;

  /** The type of event this is. */
  enum libyagbe_scheduler_event_types type;

  /** The group this event belongs to. */
  enum libyagbe_scheduler_event_groups group;
};

struct libyagbe_scheduler {
  struct libyagbe_scheduler_event events[LIBYAGBE_SCHEDULER_MAX_EVENTS];
  size_t heap_size;
  uintmax_t timestamp_now;
};

void libyagbe_scheduler_reset(void);

void libyagbe_scheduler_insert_event(struct libyagbe_scheduler_event* event);

struct libyagbe_scheduler_event* libyagbe_scheduler_find_event(
    const enum libyagbe_scheduler_event_types event);

void libyagbe_scheduler_delete_event_group(
    const enum libyagbe_scheduler_event_groups group);

void libyagbe_scheduler_add_cycles(const unsigned int cycles);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LIBYAGBE_SCHEDULER_H */