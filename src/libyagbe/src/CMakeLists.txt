# Copyright 2022 Michael Rodriguez <mike@kaichiuchu.dev>
#
# Permission to use, copy, modify, and/or distribute this software for any
# purpose with or without fee is hereby granted, provided that the above
# copyright notice and this permission notice appear in all copies.
#
# THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH
# REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY
# AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT,
# INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
# LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
# OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
# PERFORMANCE OF THIS SOFTWARE.

set(PRIVATE_SRCS private/apu.c
                 private/bus.c
                 private/cpu.c
                 private/gb.c
                 private/ppu.c
                 private/scheduler.c
                 private/timer.c)

set(PRIVATE_DEBUG_SRCS private/debug/disasm.c
                       private/debug/logger.c)

set(PRIVATE_HDRS private/utility.h)

set(PUBLIC_HDRS public/libyagbe/apu.h
                public/libyagbe/bus.h
                public/libyagbe/cpu.h
                public/libyagbe/gb.h
                public/libyagbe/ppu.h
                public/libyagbe/scheduler.h
                public/libyagbe/timer.h)

set(PUBLIC_COMPAT_HDRS public/libyagbe/compat/compat_stdbool.h
                       public/libyagbe/compat/compat_stdint.h)

set(PUBLIC_DEBUG_HDRS public/libyagbe/debug/disasm.h
                      public/libyagbe/debug/logger.h)

add_library(yagbecore STATIC ${PRIVATE_SRCS}
                             ${PRIVATE_DEBUG_SRCS}
                             ${PRIVATE_HDRS}
                             ${PUBLIC_HDRS}
                             ${PUBLIC_COMPAT_HDRS}
                             ${PUBLIC_DEBUG_HDRS})

target_include_directories(yagbecore PUBLIC public)
yagbe_configure_c_target(yagbecore)
