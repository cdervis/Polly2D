// Copyright (C) 2023-2024 Cemalettin Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE.

#pragma once

#include "Polly/Maybe.hpp"
#include "Polly/String.hpp"
#include "Polly/StringView.hpp"
#include "Polly/ToString.hpp"

namespace Polly
{
namespace Details
{
template<typename T>
static void formatHelper(String& buffer, StringView& str, const T& value)
{
    const auto openBracket = str.find('{');
    if (not openBracket)
    {
        return;
    }

    const auto closeBracket = str.find('}', *openBracket + 1);
    if (not closeBracket)
    {
        return;
    }

    buffer += str.substring(0, *openBracket);
    buffer += Polly::toString(value);

    str = str.substring(*closeBracket + 1);
}
} // namespace Details

template<typename... Args>
[[nodiscard]]
static String formatString(StringView fmt, Args&&... args)
{
    auto result = String();
    (Details::formatHelper(result, fmt, std::forward<Args>(args)), ...);
    result += fmt;
    return result;
}
} // namespace Polly
