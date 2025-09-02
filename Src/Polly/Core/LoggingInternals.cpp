// Copyright (C) 2025 Cem Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE, or https://polly2d.org.

#include "Polly/Core/LoggingInternals.hpp"

#include "Polly/Window.hpp"

#include "Polly/List.hpp"
#include <algorithm>

namespace Polly
{
static constexpr auto sMaxMessagesOnScreen    = 25;
static auto           sIsLogToWindowEnabled   = false;
static auto           sIsLoggingSuspended     = false;
static auto           sLoggedOnScreenMessages = List<OnScreenLogEntry, sMaxMessagesOnScreen>();
static auto           sWindowLoggingFontSize  = 16.0f;
static auto           sWindowLoggingPosition  = WindowLoggingPosition::TopLeft;
} // namespace Polly

bool Polly::isLogToWindowEnabled()
{
    return sIsLogToWindowEnabled;
}

void Polly::setLogToWindowEnabled(bool value)
{
    sIsLogToWindowEnabled = value;
}

void Polly::setIsLoggingSuspended(bool value)
{
    sIsLoggingSuspended = value;
}

float Polly::windowLoggingFontSize()
{
    return sWindowLoggingFontSize;
}

void Polly::setWindowLoggingFontSize(float value)
{
    sWindowLoggingFontSize = clamp(value, 10.0f, 48.0f);
}

Polly::WindowLoggingPosition Polly::windowLoggingPosition()
{
    return sWindowLoggingPosition;
}

void Polly::setWindowLoggingPosition(WindowLoggingPosition value)
{
    sWindowLoggingPosition = value;
}

Polly::Span<Polly::OnScreenLogEntry> Polly::loggedOnScreenMessages()
{
    return sLoggedOnScreenMessages;
}

void Polly::addOnScreenLogEntry(OnScreenLogEntry entry)
{
    if (sIsLoggingSuspended)
    {
        return;
    }

    if (sLoggedOnScreenMessages.size() >= sMaxMessagesOnScreen)
    {
        sLoggedOnScreenMessages.removeFirst();
    }

    sLoggedOnScreenMessages.add(std::move(entry));
}

void Polly::updateOnScreenMessages(float dt)
{
    for (OnScreenLogEntry& entry : sLoggedOnScreenMessages)
    {
        entry.ttl -= dt;
    }

    sLoggedOnScreenMessages.removeAllWhere([](const OnScreenLogEntry& entry) { return entry.ttl <= 0.0f; });
}

void Polly::clearOnScreenMessages()
{
    sLoggedOnScreenMessages.clear();
}