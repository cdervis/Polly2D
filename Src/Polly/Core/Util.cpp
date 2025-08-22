// Copyright (C) 2025 Cemalettin Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE.

#include "Polly/Util.hpp"

#include "Polly/Any.hpp"
#include "Polly/Game/GameImpl.hpp"

namespace Polly
{
String bytesDisplayString(u64 size)
{
    if (size > 1'000'000'000)
    {
        return formatString("{} GB", size / 1'000'000'000);
    }

    if (size > 1'000'000)
    {
        return formatString("{} MB", size / 1'000'000);
    }

    if (size > 1'000)
    {
        return formatString("{} KB", size / 1'000);
    }

    return formatString("{} bytes", size);
}

String toString(AnyType value)
{
    const auto info = Details::any_type_info(value);
    return info ? String(info->name) : "<unknown>";
}
} // namespace Polly