// Copyright (C) 2025 Cemalettin Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE.

#pragma once

#include "Polly/Assume.hpp"
#include "Polly/Format.hpp"
#include "Polly/Prerequisites.hpp"

namespace Polly
{
namespace Details
{
enum class LogMessageType
{
    Info    = 1,
    Warning = 2,
    Error   = 3,
};

void logInternal(String message, LogMessageType type);
} // namespace Details

/// Logs information to the system's output.
template<typename... Args>
void logInfo(StringView fmt, Args&&... args)
{
    auto str = formatString(fmt, std::forward<Args>(args)...);
    Details::logInternal(std::move(str), Details::LogMessageType::Info);
}

/// Logs a warning to the system's output.
template<typename... Args>
void logWarning(StringView fmt, Args&&... args)
{
    auto str = formatString(fmt, std::forward<Args>(args)...);
    Details::logInternal(std::move(str), Details::LogMessageType::Warning);
}

/// Logs a warning to the system's output.
template<typename... Args>
void logError(StringView fmt, Args&&... args)
{
    auto str = formatString(fmt, std::forward<Args>(args)...);
    Details::logInternal(std::move(str), Details::LogMessageType::Error);
}

/// Logs information to the system's output **in debug mode only**.
///
/// In release mode, this will result in a no-op.
template<typename... Args>
void logDebug([[maybe_unused]] StringView fmt, [[maybe_unused]] Args&&... args)
{
#if !defined(NDEBUG)
    auto str = formatString(fmt, std::forward<Args>(args)...);
    Details::logInternal(std::move(str), Details::LogMessageType::Info);
#endif
}

/// Logs information to the system's output **in debug mode only**,
/// and only if the `ENABLE_VERBOSE_LOGGING` preprocessor definition is specified.
///
/// In release mode, this will result in a no-op.
template<typename... Args>
void logVerbose(
    [[maybe_unused]] StringView fmt,
    [[maybe_unused]] Args&&... args) // NOLINT
{
#if defined(ENABLE_VERBOSE_LOGGING) && !defined(NDEBUG)
    auto str = formatString(fmt, std::forward<Args>(args)...);
    Details::logInternal(std::move(str), Details::LogMessageType::Info);
#endif
}
} // namespace Polly

#define notImplemented()                                                                                     \
    throw Error(                                                                                             \
        Polly::formatString(                                                                                 \
            "Unimplemented code reached in function '{}()', file {}, line {}.",                              \
            __FUNCTION__,                                                                                    \
            __FILE__,                                                                                        \
            __LINE__))
