// Copyright (C) 2025 Cem Dervis
// This file is part of Polly, a minimalistic 2D C++ game framework.
// For conditions of distribution and use, see copyright notice in LICENSE, or https://polly2d.org.

#pragma once

#define DeleteCopy(className)                                                                                \
    className(const className&)      = delete;                                                               \
    void operator=(const className&) = delete

#define DeleteMove(className)                                                                                \
    className(className&&) noexcept      = delete;                                                           \
    void operator=(className&&) noexcept = delete

#define DeleteCopyAndMove(className)                                                                         \
    DeleteCopy(className);                                                                                   \
    DeleteMove(className)

#if __cpp_constexpr >= 202207L
#define DefaultCopy(className)                                                                               \
    constexpr className(const className&)            = default;                                              \
    constexpr className& operator=(const className&) = default

#define DefaultMove(className)                                                                               \
    constexpr className(className&&) noexcept            = default;                                          \
    constexpr className& operator=(className&&) noexcept = default
#else
#define DefaultCopy(className)                                                                               \
    className(const className&)            = default;                                                        \
    className& operator=(const className&) = default

#define DefaultMove(className)                                                                               \
    className(className&&) noexcept            = default;                                                    \
    className& operator=(className&&) noexcept = default
#endif

#define DefaultCopyAndMove(className)                                                                        \
    DefaultCopy(className);                                                                                  \
    DefaultMove(className)
