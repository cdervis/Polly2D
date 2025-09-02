// Copyright (C) 2025 Cem Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE, or https://polly2d.org.

#pragma once

#include "Polly/Span.hpp"
#include "Polly/String.hpp"

namespace Polly
{
enum class WindowLoggingPosition;

namespace Details
{
enum class LogMessageType;
}

struct OnScreenLogEntry
{
    static constexpr auto defaultTimeToLive = 3.0f;

    String                  message;
    float                   ttl = defaultTimeToLive;
    Details::LogMessageType type;
};

bool isLogToWindowEnabled();

void setLogToWindowEnabled(bool value);

void setIsLoggingSuspended(bool value);

float windowLoggingFontSize();

void setWindowLoggingFontSize(float value);

WindowLoggingPosition windowLoggingPosition();

void setWindowLoggingPosition(WindowLoggingPosition value);

Span<OnScreenLogEntry> loggedOnScreenMessages();

void addOnScreenLogEntry(OnScreenLogEntry entry);

void updateOnScreenMessages(float dt);

void clearOnScreenMessages();
} // namespace Polly
