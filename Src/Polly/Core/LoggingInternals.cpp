// Copyright (C) 2025 Cemalettin Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE.

#include "Polly/Core/LoggingInternals.hpp"

#include "Polly/Window.hpp"

#include "Polly/List.hpp"
#include <algorithm>

namespace Polly
{
static constexpr auto maxMessagesOnScreen     = 25;
static auto           bIsLogToWindowEnabled   = false;
static auto           bIsLoggingSuspended     = false;
static auto           sLoggedOnScreenMessages = List<OnScreenLogEntry, maxMessagesOnScreen>();
static auto           sWindowLoggingFontSize  = 16.0f;
static auto           sWindowLoggingPosition  = WindowLoggingPosition::TopLeft;
} // namespace Polly

bool Polly::isLogToWindowEnabled()
{
    return bIsLogToWindowEnabled;
}

void Polly::setLogToWindowEnabled(bool value)
{
    bIsLogToWindowEnabled = value;
}

void Polly::setIsLoggingSuspended(bool value)
{
    bIsLoggingSuspended = value;
}

float Polly::windowLoggingFontSize()
{
    return sWindowLoggingFontSize;
}

void Polly::setWindowLoggingFontSize(float value)
{
    sWindowLoggingFontSize = Polly::clamp(value, 10.0f, 48.0f);
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
    if (bIsLoggingSuspended)
    {
        return;
    }

    if (sLoggedOnScreenMessages.size() >= maxMessagesOnScreen)
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