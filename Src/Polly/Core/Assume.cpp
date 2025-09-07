// Copyright (C) 2025 Cem Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE, or https://polly2d.org.

#include "Polly/Assume.hpp"

#include "Polly/Core/LoggingInternals.hpp"
#include "Polly/Defer.hpp"
#include "Polly/Error.hpp"
#include "Polly/Logging.hpp"

#ifndef NDEBUG

void Polly::Details::assumptionViolation(
    const char* filename,
    const char* function,
    int         line,
    const char* predicateStr,
    const char* message)
{
    setLogToWindowEnabled(false);

    defer
    {
        setLogToWindowEnabled(true);
    };

    if (message)
    {
        if (const auto messageStr = StringView(message); !messageStr.isEmpty())
        {
            throw Error(formatString(
                "Assumption violated in {}, function '{}', line {}\n    Condition: {}\n    Message: {}",
                filename,
                function,
                line,
                predicateStr,
                messageStr));
        }
    }

    throw Error(formatString(
        "Assumption violated at {}, function '{}', line {}\n    Condition: {}",
        filename,
        function,
        line,
        predicateStr));
}

#endif
