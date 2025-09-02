// Copyright (C) 2025 Cem Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE.

#pragma once

#include "Polly/CopyMoveMacros.hpp"
#include "Polly/Logging.hpp"
#include <cstdint>

#define pl_implement_object_funcs(className)                                                                 \
    className::className(Impl* impl)                                                                         \
        : _impl(impl)                                                                                        \
    {                                                                                                        \
        if (_impl)                                                                                           \
        {                                                                                                    \
            _impl->addRef();                                                                                 \
        }                                                                                                    \
    }                                                                                                        \
    className::className(const className& copyFrom)                                                          \
        : _impl(copyFrom._impl)                                                                              \
    {                                                                                                        \
        if (_impl)                                                                                           \
        {                                                                                                    \
            _impl->addRef();                                                                                 \
        }                                                                                                    \
    }                                                                                                        \
    className& className::operator=(const className& copyFrom)                                               \
    {                                                                                                        \
        if (std::addressof(copyFrom) != this)                                                                \
        {                                                                                                    \
            if (_impl)                                                                                       \
            {                                                                                                \
                _impl->release();                                                                            \
            }                                                                                                \
                                                                                                             \
            _impl = copyFrom._impl;                                                                          \
                                                                                                             \
            if (_impl)                                                                                       \
            {                                                                                                \
                _impl->addRef();                                                                             \
            }                                                                                                \
        }                                                                                                    \
                                                                                                             \
        return *this;                                                                                        \
    }                                                                                                        \
    className::className(className&& moveFrom) noexcept                                                      \
        : _impl(moveFrom._impl)                                                                              \
    {                                                                                                        \
        moveFrom._impl = nullptr;                                                                            \
    }                                                                                                        \
    className& className::operator=(className&& moveFrom) noexcept                                           \
    {                                                                                                        \
        if (std::addressof(moveFrom) != this)                                                                \
        {                                                                                                    \
            if (_impl)                                                                                       \
            {                                                                                                \
                _impl->release();                                                                            \
            }                                                                                                \
                                                                                                             \
            _impl          = moveFrom._impl;                                                                 \
            moveFrom._impl = nullptr;                                                                        \
        }                                                                                                    \
                                                                                                             \
        return *this;                                                                                        \
    }                                                                                                        \
    className::~className() noexcept                                                                         \
    {                                                                                                        \
        if (_impl)                                                                                           \
        {                                                                                                    \
            _impl->release();                                                                                \
        }                                                                                                    \
    }

#define PollyVerifyObject(T)                                                                                 \
    static_assert(sizeof(T) == sizeof(uintptr_t), "The type must be a Polly object without any extra fields.")

#define PollyImplementObject(className)                                                                      \
    PollyVerifyObject(className);                                                                            \
    pl_implement_object_funcs(className)

#define PollyVerifyImplAccess                                                                                \
    if (not impl)                                                                                            \
    {                                                                                                        \
        throw Error(formatString("[{}] Attempting to access an empty object.", __FUNCTION__));               \
    }

#define PollyDeclareThisImpl                                                                                 \
    const auto impl = this->impl();                                                                          \
    PollyVerifyImplAccess

#define PollyDeclareThisImplNoVerify const auto impl = this->impl();

#define PollyVerifyHaveImpl                                                                                  \
    if (not impl())                                                                                          \
    {                                                                                                        \
        throw Error(formatString("[{}] Attempting to access and empty object.", __FUNCTION__));              \
    }

#define PollyDeclareThisImplAs(type)                                                                         \
    auto* const impl = static_cast<type*>(this->impl());                                                     \
    PollyVerifyImplAccess

namespace Polly
{
class Object
{
  public:
    explicit Object() = default;

    DeleteCopyAndMove(Object);

    virtual ~Object() noexcept = default;

    void addRef();

    u64 release();

    u64 refCount() const;

  private:
    u64 _refCount = 0;
};

template<typename T>
struct ObjectLayout
{
    PollyVerifyObject(T);

    typename T::Impl* impl{};
};

template<typename T, typename TImpl = typename T::Impl>
static void setImpl(T& obj, TImpl* impl)
{
    auto& s = reinterpret_cast<ObjectLayout<T>&>(obj);

    if (s.impl)
    {
        s.impl->release();
    }

    s.impl = impl;

    if (s.impl)
    {
        s.impl->addRef();
    }
}
} // namespace Polly
