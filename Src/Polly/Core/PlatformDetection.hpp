// Copyright (C) 2025 Cem Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE, or https://polly2d.org.

// Internal header that can be used to query information about the current target platform
// at compile time.
// Usage:
// #if polly_platform_linux
// ...
// #endif

#pragma once

#ifdef __APPLE__
#include <TargetConditionals.h>
#endif

// clang-format off

#if defined(__ANDROID__)
#  define polly_platform_android 1 // NOLINT(*-macro-usage)
#elif defined(__linux__)
#  define polly_platform_linux 1 // NOLINT(*-macro-usage)
#endif

#if defined(_WIN32) || defined(_WIN64)
#  define polly_platform_windows 1 // NOLINT(*-macro-usage)
#endif

#if defined(__APPLE__)
#  if TARGET_OS_IOS
#    define polly_platform_ios 1 // NOLINT(*-macro-usage)
#  elif TARGET_OS_OSX
#    define polly_platform_macos 1 // NOLINT(*-macro-usage)
#  endif
#endif

#if defined(__clang__) || defined(__llvm__)
#  define POLLY_COMPILER_CLANG 1 // NOLINT(*-macro-usage)
#endif

#if defined(__GNUC__)
#  define POLLY_COMPILER_GCC 1 // NOLINT(*-macro-usage)
#endif

#if defined(_MSC_VER)
#  define POLLY_COMPILER_MSVC 1 // NOLINT(*-macro-usage)
#endif

// clang-format on
