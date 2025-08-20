// Copyright (C) 2025 Cemalettin Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE.

#pragma once

#include <type_traits>

// TODO: C++23 supports the static call operator, which we can take advantage of by feature-checking against
// '__cpp_static_call_operator'.

#define polly_define_comparer(name, op)                                                                      \
    template<typename T, typename U = T>                                                                     \
    struct name                                                                                              \
    {                                                                                                        \
        constexpr bool operator()(const T& lhs, const U& rhs) const                                          \
        {                                                                                                    \
            return lhs op rhs;                                                                               \
        }                                                                                                    \
        constexpr bool operator()(const U& lhs, const T& rhs) const                                          \
        requires(!std::is_same_v<T, U>)                                                                      \
        {                                                                                                    \
            return lhs op rhs;                                                                               \
        }                                                                                                    \
    }

namespace Polly::Comparers
{
polly_define_comparer(Equal, ==);
polly_define_comparer(Less, <);
polly_define_comparer(LessEqual, <=);
polly_define_comparer(Greater, >);
polly_define_comparer(GreaterEqual, >=);
} // namespace Polly::comparers

#undef polly_define_comparer
