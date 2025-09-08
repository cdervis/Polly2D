// Copyright (C) 2025 Cem Dervis
// This file is part of Polly, a minimalistic 2D C++ game framework.
// For conditions of distribution and use, see copyright notice in LICENSE, or https://polly2d.org.

#pragma once

#include "Polly/CopyMoveMacros.hpp"
#include <exception>

namespace Polly
{
class String;
class StringView;

/// Represents a general error that is raised by Polly.
class Error : public std::exception
{
  public:
    explicit Error(const char* message);

    explicit Error(String message);

    explicit Error(StringView message);

    Error(const Error& copyFrom);

    Error& operator=(const Error& copyFrom);

    Error(Error&& moveFrom) noexcept;

    Error& operator=(Error&& moveFrom) noexcept;

    ~Error() noexcept override;

    const char* what() const noexcept override;

    StringView message() const;

  private:
    class Impl;
    Impl* _impl;
};
} // namespace Polly