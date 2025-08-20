// Copyright (C) 2025 Cemalettin Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE.

#pragma once

#define deleteCopy(class_name)                                                                               \
    class_name(const class_name&)     = delete;                                                              \
    void operator=(const class_name&) = delete

#define deleteMove(class_name)                                                                               \
    class_name(class_name&&) noexcept     = delete;                                                          \
    void operator=(class_name&&) noexcept = delete

#define deleteCopyAndMove(class_name)                                                                        \
    deleteCopy(class_name);                                                                                  \
    deleteMove(class_name)

#if __cpp_constexpr >= 202207L
#define defaultCopy(class_name)                                                                              \
    constexpr class_name(const class_name&)            = default;                                            \
    constexpr class_name& operator=(const class_name&) = default

#define defaultMove(class_name)                                                                              \
    constexpr class_name(class_name&&) noexcept            = default;                                        \
    constexpr class_name& operator=(class_name&&) noexcept = default
#else
#define defaultCopy(class_name)                                                                              \
    class_name(const class_name&)            = default;                                                      \
    class_name& operator=(const class_name&) = default

#define defaultMove(class_name)                                                                              \
    class_name(class_name&&) noexcept            = default;                                                  \
    class_name& operator=(class_name&&) noexcept = default
#endif

#define defaultCopyAndMove(class_name)                                                                       \
    defaultCopy(class_name);                                                                                 \
    defaultMove(class_name)
