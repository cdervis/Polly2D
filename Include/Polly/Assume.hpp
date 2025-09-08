// Copyright (C) 2023-2024 Cem Dervis
// This file is part of Polly, a minimalistic 2D C++ game framework.
// For conditions of distribution and use, see copyright notice in LICENSE, or https://polly2d.org.

#pragma once

namespace Polly::Details
{
[[noreturn]]
void assumptionViolation(
    const char* filename,
    const char* function,
    int         line,
    const char* predicateStr,
    const char* message);
} // namespace Polly::Details

// clang-format off

#ifndef NDEBUG
#  if defined(_WIN32) || defined(_WIN64)
#    define debugBreak() __debugbreak()
#  elif defined(__clang__) && __has_builtin(__builtin_debugtrap)
#    define debugBreak() __builtin_debugtrap()
#  else
   #include <signal.h>
   __attribute__((always_inline)) __inline__ static void debug_break() {
       ::raise(SIGTRAP);
   }
#  endif
#else
#  define debugBreak()
#endif

// clang-format on

#ifndef NDEBUG
#ifdef __GNUC__

#define assume(expr)                                                                                         \
    if (not(expr))                                                                                           \
    {                                                                                                        \
        ::Polly::Details::assumptionViolation(__FILE__, __PRETTY_FUNCTION__, __LINE__, #expr, "");           \
    }

#define assumeWithMsg(expr, message)                                                                         \
    if (not(expr))                                                                                           \
    {                                                                                                        \
        ::Polly::Details::assumptionViolation(__FILE__, __PRETTY_FUNCTION__, __LINE__, #expr, message);      \
    }

#else

#define assume(expr)                                                                                         \
    if (not(expr))                                                                                           \
    {                                                                                                        \
        ::Polly::Details::assumptionViolation(__FILE__, __FUNCTION__, __LINE__, #expr, "");                  \
    }

#define assumeWithMsg(expr, message)                                                                         \
    if (not(expr))                                                                                           \
    {                                                                                                        \
        ::Polly::Details::assumptionViolation(__FILE__, __FUNCTION__, __LINE__, #expr, message);             \
    }

#endif
#else
#define assume(expr)
#define assumeWithMsg(expr, message)
#endif
