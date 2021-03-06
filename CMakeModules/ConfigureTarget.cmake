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

function(set_build_flags TARGET_NAME)
  if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    if (CMAKE_C_COMPILER_ID MATCHES "Clang")
      if (CMAKE_CXX_COMPILER_FRONTEND_VARIANT MATCHES "GNU")
        set(COMPILER_FLAGS "-pedantic;-pedantic-errors;-Wall;-Werror;-Wextra")
      else()
        set(COMPILER_FLAGS "-Wall;-Werror;-Wextra")
      endif()
    elseif (CMAKE_C_COMPILER_ID MATCHES "GNU")
      set(COMPILER_FLAGS "-pedantic;-pedantic-errors;-Wall;-Werror;-Wextra")
    endif()

    target_compile_options(${TARGET_NAME} PRIVATE ${COMPILER_FLAGS}) 
  elseif (CMAKE_BUILD_TYPE MATCHES "Release|RelWithDebInfo|MinSizeRel")
    include(CheckIPOSupported)
    check_ipo_supported(RESULT ipo_supported)

    if (ipo_supported)
      set_target_properties(${TARGET_NAME} PROPERTIES
                            INTERPROCEDURAL_OPTIMIZATION TRUE)
    endif()
  endif()
endfunction()

function(yagbe_configure_c_target TARGET_NAME)
  set_target_properties(${TARGET_NAME} PROPERTIES
                        C_STANDARD 90
                        C_STANDARD_REQUIRED YES
                        C_EXTENSIONS NO)

  set_build_flags(${TARGET_NAME})
endfunction()

function(yagbe_configure_cpp_target TARGET_NAME)
  set_target_properties(${TARGET_NAME} PROPERTIES
                        CXX_STANDARD 17
                        CXX_STANDARD_REQUIRED YES
                        CXX_EXTENSIONS YES)

  set_build_flags(${TARGET_NAME})
endfunction()