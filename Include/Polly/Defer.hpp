// Copyright (C) 2025 Cemalettin Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE.

// This file contains the `defer` macro which can be used to perform
// guaranteed, arbitrary scope-based cleanup.
//
// Example usage:
//
// void someFunction()
// {
//     defer
//     {
//         logDebug("End of scope reached!");
//     };
//
//     logDebug("Beginning some_function");
// }
//
// Which would print the following:
//   Beginning some_function
//   End of scope reached!

#pragma once

#include "Polly/CopyMoveMacros.hpp"
#include <utility>

// NOLINTBEGIN
#define POLLY_CONCAT_INNER(a, b) a##b
#define POLLY_CONCAT(a, b)       POLLY_CONCAT_INNER(a, b)
#define POLLY_UNIQUE_NAME(base)  POLLY_CONCAT(base, __COUNTER__)
// NOLINTEND

namespace Polly::Details
{
template<typename Functor>
class DeferObject final
{
  public:
    DeferObject() = delete;

    explicit DeferObject(Functor action)
        : _functor(action)
        , _isDismissed(true)
    {
    }

    deleteCopy(DeferObject);

    DeferObject(DeferObject&& other) noexcept
        : _functor(std::move(other._functor))
        , _isDismissed(other._isDismissed)
    {
        other._isDismissed = true;
    }

    void operator=(DeferObject&&) noexcept = delete;

    ~DeferObject()
    {
        if (_isDismissed)
            _functor();
    }

    void dismiss()
    {
        _isDismissed = false;
    }

  private:
    Functor _functor;
    bool    _isDismissed = false;
};

enum class DeferOperatorOverloadTag
{
};

template<typename Functor>
DeferObject<Functor> operator+(DeferOperatorOverloadTag, Functor&& functor)
{
    return DeferObject<Functor>(std::forward<Functor>(functor));
}
} // namespace Polly::Details

// NOLINTBEGIN

#define deferNamed(name)                                                                                     \
    auto name = Polly::Details::DeferOperatorOverloadTag{} + [&]() // NOLINT(*-macro-parentheses)

#define defer deferNamed(POLLY_UNIQUE_NAME(DEFER_GUARD))

// NOLINTEND
