// Copyright (C) 2025 Cem Dervis
// This file is part of Polly, a 2D C++ game framework for minimalists.
// For conditions of distribution and use, see copyright notice in LICENSE, or https://polly2d.org.

// This file uses code from the Guidelines Support Library by Microsoft, licensed under the MIT license:

///////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2015 Microsoft Corporation. All rights reserved.
//
// This code is licensed under the MIT License (MIT).
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//
///////////////////////////////////////////////////////////////////////////////

#pragma once

#include "Polly/Error.hpp"
#include <concepts>

namespace Polly
{
/// A checked cast that throws an error if the cast changes the value.
template<typename T, typename U>
requires std::is_arithmetic_v<T>
constexpr T narrow(U u)
{
    constexpr auto isDifferentSignedness = (std::is_signed_v<T> != std::is_signed_v<U>);

    // While this is technically undefined behavior in some cases (i.e., if the source value is of
    // floating-point type and cannot fit into the destination integral type), the resultant behavior
    // is benign on the platforms that we target (i.e., no hardware trap representations are hit).
    const auto t = static_cast<T>(u);

#if defined(__clang__) or defined(__GNUC__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wfloat-equal"
#endif

    // Note: NaN will always throw, since NaN != NaN
    if (static_cast<U>(t) != u or (isDifferentSignedness and ((t < T()) != (u < U()))))
    {
        throw Error("Narrow casting error.");
    }

#if defined(__clang__) or defined(__GNUC__)
#pragma GCC diagnostic pop
#endif

    return t;
}

/// A checked cast that throws an error if the cast changes the value.
template<typename T, typename U>
requires(not std::is_arithmetic_v<T>)
constexpr T narrow(U u)
{
    const auto t = static_cast<T>(u);

    if (static_cast<U>(t) != u)
    {
        throw Error("Narrow casting error.");
    }

    return t;
}
} // namespace Polly