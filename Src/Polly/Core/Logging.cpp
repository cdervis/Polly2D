// Copyright (C) 2025 Cem Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE.

#include "Polly/Logging.hpp"
#include "Polly/Core/LoggingInternals.hpp"
#include "Polly/Core/PlatformDetection.hpp"
#include <cstdint>

#if polly_platform_windows
#include <Windows.h>
#elif polly_platform_android
#include <android/log.h>
#endif

namespace Polly
{
void Details::logInternal(String message, LogMessageType type)
{
    if (message.isEmpty())
    {
        return;
    }

    if (not isLogToWindowEnabled())
    {
#if polly_platform_windows
        auto fullMsg = String();
        fullMsg.reserve(message.size() + 1);

        switch (type)
        {
            case LogMessageType::Warning: fullMsg = "WARNING: "; break;
            default: break;
        }

        fullMsg += message;
        fullMsg += '\n';

        OutputDebugString(fullMsg.cstring());
#elif polly_platform_android
        const auto log_priority = [type]
        {
            switch (type)
            {
                case LogMessageType::Info: return ANDROID_LOG_INFO;
                case LogMessageType::Warning: return ANDROID_LOG_WARN;
                case LogMessageType::Error: return ANDROID_LOG_ERROR;
            }

            return ANDROID_LOG_INFO;
        }();

        __android_log_print(log_priority, "Polly", "%s", message.cstring());
#else
        switch (type)
        {
            case LogMessageType::Info: {
                fprintf(stdout, "%s\n", message.cstring());
                fflush(stdout);
                break;
            }
            case LogMessageType::Warning: {
                fprintf(stderr, "WARNING: %s\n", message.cstring());
                break;
            }
            case LogMessageType::Error: {
                fprintf(stderr, "ERROR: %s\n", message.cstring());
                break;
            }
        }
#endif
    }
    else
    {
        addOnScreenLogEntry(
            OnScreenLogEntry{
                .message = std::move(message),
                .type    = type,
            });
    }
}
} // namespace Polly