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

#include "libyagbe/scheduler.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "libyagbe/debug/logger.h"
#include "libyagbe/timer.h"
#include "utility.h"

static struct libyagbe_scheduler scheduler;

static char* get_event_name(const enum libyagbe_scheduler_event_types type) {
  size_t alloc_size;
  char* str;

  switch (type) {
    case LIBYAGBE_SCHEDULER_EVENT_TIMA_INCREMENT:
      alloc_size = strlen("TIMA increment event");
      str = malloc(sizeof(char) * alloc_size);
      strcpy(str, "TIMA increment event");

      return str;

    case LIBYAGBE_SCHEDULER_EVENT_TIMA_OVERFLOW:
      alloc_size = strlen("TIMA overflow event");
      str = malloc(sizeof(char) * alloc_size);

      strcpy(str, "TIMA overflow event");
      return str;

    default:
      return NULL;
  }
}

static size_t get_parent_node(const size_t index) { return (index - 1) / 2; }

static size_t get_left_child_of_node(const size_t index) {
  return (index * 2) + 1;
}

static size_t get_right_child_of_node(const size_t index) {
  return (index * 2) + 2;
}

static void heapify_top_bottom(const size_t parent_node) {
  size_t left_node;
  size_t right_node;
  size_t smallest_node;

  left_node = get_left_child_of_node(parent_node);
  right_node = get_right_child_of_node(parent_node);
  smallest_node = parent_node;

  if ((smallest_node < scheduler.heap_size) &&
      (scheduler.events[left_node].timestamp <
       scheduler.events[smallest_node].timestamp)) {
    smallest_node = left_node;
  }

  if ((right_node < scheduler.heap_size) &&
      scheduler.events[right_node].timestamp <
          scheduler.events[smallest_node].timestamp) {
    smallest_node = right_node;
  }

  if (smallest_node != parent_node) {
    SWAP(scheduler.events[smallest_node], scheduler.events[parent_node],
         struct libyagbe_scheduler_event);

    heapify_top_bottom(smallest_node);
  }
}

static void heapify_bottom_top(const size_t index) {
  size_t parent_node;

  parent_node = get_parent_node(index);

  if (scheduler.events[parent_node].timestamp >
      scheduler.events[index].timestamp) {
    SWAP(scheduler.events[parent_node], scheduler.events[index],
         struct libyagbe_scheduler_event);
    heapify_bottom_top(parent_node);
  }
}

static void delete_min(void) {
  assert(scheduler.heap_size != 0);

  scheduler.events[0] = scheduler.events[--scheduler.heap_size];
  heapify_top_bottom(0);
}

static void step(const uintmax_t timestamp_next) {
  while ((scheduler.events[0].timestamp <= timestamp_next) &&
         scheduler.heap_size > 0) {
    scheduler.events[0].cb_func();
    scheduler.timestamp_now = scheduler.events[0].timestamp;
    delete_min();
  }
}

void libyagbe_scheduler_reset(void) {
  memset(&scheduler, 0, sizeof(struct libyagbe_scheduler));
  libyagbe_log(LIBYAGBE_LOG_LEVEL_INFO, "Resetting scheduler.");
}

void libyagbe_scheduler_insert_event(
    struct libyagbe_scheduler_event* const event) {
  assert(event != NULL);
  assert(scheduler.heap_size != (LIBYAGBE_SCHEDULER_MAX_EVENTS - 1));

  memcpy(&scheduler.events[scheduler.heap_size], &event,
         sizeof(struct libyagbe_scheduler_event));

  heapify_bottom_top(scheduler.heap_size);
  scheduler.heap_size++;
}

struct libyagbe_scheduler_event* libyagbe_scheduler_find_event(
    const enum libyagbe_scheduler_event_types event) {
  size_t index;

  for (index = 0; index < scheduler.heap_size; ++index) {
    if (scheduler.events[index].type == event) {
      return &scheduler.events[index];
    }
  }
  return NULL;
}

void libyagbe_scheduler_delete_event_group(
    const enum libyagbe_scheduler_event_groups group) {
  size_t index;

  for (index = 0; index < scheduler.heap_size; ++index) {
    if (scheduler.events[index].group == group) {
      /*delete_by_index(index);*/
    }
  }
}

void libyagbe_scheduler_add_cycles(const unsigned int cycles) {
  const uintmax_t timestamp_next = scheduler.timestamp_now + cycles;
  step(timestamp_next);
  scheduler.timestamp_now = timestamp_next;
}