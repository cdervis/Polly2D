// Copyright (C) 2025 Cemalettin Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE.

#include "Polly/Error.hpp"

#include "Polly/Logging.hpp"
#include "Polly/Maybe.hpp"
#include "Polly/String.hpp"

namespace Polly
{
class Error::Impl final
{
  public:
    String message;
};

Error::Error(const char* message)
    : _impl(new Impl())
{
    _impl->message = message;
    logError("Error: {}", message);
}

Error::Error(String message)
    : _impl(new Impl())
{
    _impl->message = std::move(message);
    logError("Error: {}", message);
}

Error::Error(StringView message)
    : _impl(new Impl())
{
    _impl->message = message;
    logError("Error: {}", message);
}

Error::Error(const Error& copyFrom)
    : _impl(new Impl())
{
    _impl->message = copyFrom._impl->message;
}

Error& Error::operator=(const Error& copyFrom)
{
    if (&copyFrom != this)
    {
        if (not _impl)
        {
            _impl = new Impl();
        }

        _impl->message = copyFrom._impl->message;
    }

    return *this;
}

Error::Error(Error&& moveFrom) noexcept
    : _impl(std::exchange(moveFrom._impl, nullptr))
{
}

Error& Error::operator=(Error&& moveFrom) noexcept
{
    if (&moveFrom != this)
    {
        delete _impl;
        _impl = std::exchange(moveFrom._impl, nullptr);
    }

    return *this;
}

Error::~Error() noexcept
{
    delete _impl;
}

const char* Error::what() const noexcept
{
    return _impl->message.cstring();
}

StringView Error::message() const
{
    return _impl->message;
}
} // namespace Polly