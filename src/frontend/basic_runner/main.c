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

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#include "libyagbe/bus.h"
#include "libyagbe/compat/compat_stdbool.h"
#include "libyagbe/compat/compat_stdint.h"
#include "libyagbe/cpu.h"
#include "libyagbe/debug/logger.h"
#include "libyagbe/gb.h"

static bool running = true;
static uint16_t current_pc = 0x0000;

static uint8_t* open_rom(const char* const file_name) {
  FILE* rom_file;
  uint8_t* rom_data;
  struct stat st;
  size_t rom_size;

  rom_file = fopen(file_name, "rb");

  if (!rom_file) {
    fprintf(stderr, "unable to open ROM file %s: %s\n", file_name,
            strerror(errno));
    return NULL;
  }

  stat(file_name, &st);
  rom_size = st.st_size;

  rom_data = malloc(sizeof(uint8_t) * rom_size);

  fread(rom_data, 1, rom_size, rom_file);
  fclose(rom_file);

  return rom_data;
}

static void info_log_handler(const char* const msg) {
  printf("[INFO]: %s\n", msg);
}

static void warning_log_handler(const char* const msg) {
  printf("[WARNING]: %s\n", msg);
}

static void critical_log_handler(const char* const msg) {
  running = false;
  printf("[CRITICAL]: %s\n", msg);
}

int main(int argc, char* argv[]) {
  uint8_t* rom_data;
  struct libyagbe_cpu* cpu;
  FILE* trace_file;

  if (argc < 2) {
    fprintf(stderr, "%s: missing required argument.\n", argv[0]);
    fprintf(stderr, "%s: syntax: %s rom_file\n", argv[0], argv[0]);

    return EXIT_FAILURE;
  }

  rom_data = open_rom(argv[1]);

  if (rom_data == NULL) {
    return EXIT_FAILURE;
  }

  libyagbe_logger_set_log_level_cb(LIBYAGBE_LOG_LEVEL_INFO, &info_log_handler);
  libyagbe_logger_set_log_level_cb(LIBYAGBE_LOG_LEVEL_WARNING,
                                   &warning_log_handler);
  libyagbe_logger_set_log_level_cb(LIBYAGBE_LOG_LEVEL_CRITICAL,
                                   &critical_log_handler);

  libyagbe_bus_set_cart_data(rom_data);
  cpu = libyagbe_cpu_get_data();

  libyagbe_system_reset();

  trace_file = fopen("trace.txt", "w");

  while (running) {
    current_pc = cpu->reg.pc.value;

    fprintf(trace_file, "BC=%04X DE=%04X HL=%04X AF=%04X SP=%04X PC=%04X\n",
            cpu->reg.bc.value, cpu->reg.de.value, cpu->reg.hl.value,
            cpu->reg.af.value, cpu->reg.sp.value, cpu->reg.pc.value);

    if (current_pc == 0xC8B0) {
      running = false;
      break;
    }
    libyagbe_system_step();
  }

  fclose(trace_file);
  return EXIT_SUCCESS;
}