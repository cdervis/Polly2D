// Copyright (C) 2025 Cem Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE.

#pragma once

#include <memory>

namespace Polly
{
template<typename T>
class Function;

template<typename R, typename... Params>
class Function<R(Params...)> final
{
  public:
    Function()
        : _callable(nullptr)
    {
    }

    // ReSharper disable once CppNonExplicitConvertingConstructor
    template<typename Functor>
    Function(Functor functor) // NOLINT(*-explicit-constructor)
        : _callable(new CallableImpl<Functor>(std::move(functor)))
    {
    }

    Function(const Function& copyFrom)
        : _callable(copyFrom._callable->clone())
    {
    }

    Function& operator=(const Function& copyFrom)
    {
        if (std::addressof(copyFrom) != this)
        {
            destroy();
            _callable = copyFrom._callable->clone();
        }

        return *this;
    }

    Function(Function&& moveFrom) noexcept
        : _callable(moveFrom._callable)
    {
        moveFrom._callable = nullptr;
    }

    Function& operator=(Function&& moveFrom) noexcept
    {
        if (std::addressof(moveFrom) != this)
        {
            destroy();
            _callable          = moveFrom._callable;
            moveFrom._callable = nullptr;
        }

        return *this;
    }

    ~Function() noexcept
    {
        destroy();
    }

    R operator()(Params... args) const
    {
        return _callable->call(std::forward<Params>(args)...);
    }

    explicit operator bool() const
    {
        return _callable != nullptr;
    }

  private:
    struct CallableInterface
    {
        virtual R call(Params...) = 0;

        virtual ~CallableInterface() noexcept = default;

        virtual CallableInterface* clone() const = 0;
    };

    template<typename Callable>
    struct CallableImpl final : CallableInterface
    {
        explicit CallableImpl(Callable callable)
            : _callable(std::move(callable))
        {
        }

        R call(Params... param) override
        {
            return _callable(std::forward<Params>(param)...);
        }

        CallableInterface* clone() const override
        {
            return new CallableImpl(*this);
        }

      private:
        Callable _callable;
    };

    void destroy()
    {
        delete _callable;
        _callable = nullptr;
    }

    CallableInterface* _callable;
};
} // namespace Polly
