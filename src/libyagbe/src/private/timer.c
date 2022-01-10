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

#include "libyagbe/timer.h"

#include "libyagbe/bus.h"
#include "libyagbe/compat/compat_stdbool.h"
#include "libyagbe/debug/logger.h"
#include "libyagbe/scheduler.h"
#include "utility.h"

static struct libyagbe_timer timer;
static const unsigned int timing[4] = {1024, 256, 16, 8};

static void handle_tima_increment(void);
static void handle_tima_overflow(void);

static void set_tac_value(const uint8_t new_tac) {
  timer.tac = (timer.tac & ~0x07) | (new_tac & 0x07);
}

static bool timer_is_enabled(void) {
  return BIT_IS_SET(timer.tac, LIBYAGBE_TIMER_TAC_ENABLED);
}

static void insert_tima_increment_event(void) {
  struct libyagbe_scheduler_event event;

  event.timestamp = timing[timer.tac & LIBYAGBE_TIMER_TAC_CLOCK_MASK];
  event.cb_func = &handle_tima_increment;
  event.type = LIBYAGBE_SCHEDULER_EVENT_TIMA_INCREMENT;
  event.group = LIBYAGBE_SCHEDULER_EVENT_GROUP_TIMER;

  libyagbe_scheduler_insert_event(&event);
}

static void insert_tima_overflow_event(void) {
  struct libyagbe_scheduler_event event;

  event.timestamp = timing[timer.tac & LIBYAGBE_TIMER_TAC_CLOCK_MASK];
  event.cb_func = &handle_tima_overflow;
  event.type = LIBYAGBE_SCHEDULER_EVENT_TIMA_OVERFLOW;
  event.group = LIBYAGBE_SCHEDULER_EVENT_GROUP_TIMER;

  libyagbe_scheduler_insert_event(&event);
}

#if 0
static void adjust_tima_overflow_timestamp(const uint8_t new_tima_value) {
  (void)new_tima_value;
  struct libyagbe_scheduler_event* event =
      libyagbe_scheduler_find_event(LIBYAGBE_SCHEDULER_EVENT_TIMA_OVERFLOW);
  uintmax_t delta;

  /* This event should be guaranteed to be present. */
  assert(event != NULL);

  delta = (0xFF - new_tima_value) *
          timing[timer.tac & LIBYAGBE_TIMER_TAC_CLOCK_MASK];
}
#endif

static void handle_tima_overflow(void) {
  timer.tima = timer.tma;
  libyagbe_bus_set_interrupt(LIBYAGBE_BUS_IF_TIMER);

  if (timer_is_enabled()) {
    insert_tima_overflow_event();
  }
}

static void handle_tima_increment(void) {
  timer.tima++;

  if (timer_is_enabled()) {
    insert_tima_increment_event();
  }
}

void libyagbe_timer_reset(void) {
  timer.tima = 0x00;
  timer.tma = 0x00;
  timer.tac = 0xF8;
}

void libyagbe_timer_handle_tima_write(const uint8_t new_tima_value) {
  if (timer_is_enabled()) {
    /* The timer is enabled, which means a TIMA overflow event is already
     * present. We need to adjust the expiry time for it. */
    /*adjust_tima_overflow_timestamp(new_tima_value);*/
  }
  timer.tima = new_tima_value;
}

void libyagbe_timer_handle_tma_write(const uint8_t new_tma_value) {
  timer.tma = new_tma_value;
}

void libyagbe_timer_handle_tac_write(const uint8_t new_tac_value) {
  /* Is the timer being enabled from a disabled state? */
  if (!timer_is_enabled() &&
      BIT_IS_SET(new_tac_value, LIBYAGBE_TIMER_TAC_ENABLED)) {
    libyagbe_log(LIBYAGBE_LOG_LEVEL_INFO, "Timer became enabled.");

    /* The timer has now been enabled from a previously disabled state,
     * schedule the appropriate events. */
    insert_tima_increment_event();
    insert_tima_overflow_event();

    set_tac_value(new_tac_value);
    return;
  }

  /* Is the timer being disabled from an enabled state? */
  if (timer_is_enabled() &&
      !BIT_IS_SET(new_tac_value, LIBYAGBE_TIMER_TAC_ENABLED)) {
    libyagbe_log(LIBYAGBE_LOG_LEVEL_INFO, "Timer became disabled.");

    libyagbe_scheduler_delete_event_group(LIBYAGBE_SCHEDULER_EVENT_GROUP_TIMER);
    set_tac_value(new_tac_value);

    return;
  }
}
