// Copyright (C) 2025 Cemalettin Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE.

#pragma once

#include "Polly/CopyMoveMacros.hpp"
#include "Polly/Error.hpp"
#include "Polly/Logging.hpp"
#include <cstdint>

#define pl_implement_object_funcs(className)                                                                 \
    className::className(Impl* impl)                                                                         \
        : m_impl(impl)                                                                                       \
    {                                                                                                        \
        if (m_impl)                                                                                          \
        {                                                                                                    \
            m_impl->addRef();                                                                                \
        }                                                                                                    \
    }                                                                                                        \
    className::className(const className& copyFrom)                                                          \
        : m_impl(copyFrom.m_impl)                                                                            \
    {                                                                                                        \
        if (m_impl)                                                                                          \
        {                                                                                                    \
            m_impl->addRef();                                                                                \
        }                                                                                                    \
    }                                                                                                        \
    className& className::operator=(const className& copyFrom)                                               \
    {                                                                                                        \
        if (std::addressof(copyFrom) != this)                                                                \
        {                                                                                                    \
            if (m_impl)                                                                                      \
            {                                                                                                \
                m_impl->release();                                                                           \
            }                                                                                                \
                                                                                                             \
            m_impl = copyFrom.m_impl;                                                                        \
                                                                                                             \
            if (m_impl)                                                                                      \
            {                                                                                                \
                m_impl->addRef();                                                                            \
            }                                                                                                \
        }                                                                                                    \
                                                                                                             \
        return *this;                                                                                        \
    }                                                                                                        \
    className::className(className&& moveFrom) noexcept                                                      \
        : m_impl(moveFrom.m_impl)                                                                            \
    {                                                                                                        \
        moveFrom.m_impl = nullptr;                                                                           \
    }                                                                                                        \
    className& className::operator=(className&& moveFrom) noexcept                                           \
    {                                                                                                        \
        if (std::addressof(moveFrom) != this)                                                                \
        {                                                                                                    \
            if (m_impl)                                                                                      \
            {                                                                                                \
                m_impl->release();                                                                           \
            }                                                                                                \
                                                                                                             \
            m_impl          = moveFrom.m_impl;                                                               \
            moveFrom.m_impl = nullptr;                                                                       \
        }                                                                                                    \
                                                                                                             \
        return *this;                                                                                        \
    }                                                                                                        \
    className::~className() noexcept                                                                         \
    {                                                                                                        \
        if (m_impl)                                                                                          \
        {                                                                                                    \
            m_impl->release();                                                                               \
        }                                                                                                    \
    }

#define verify_polly_object(T)                                                                               \
    static_assert(sizeof(T) == sizeof(uintptr_t), "The type must be a Polly object without any extra fields.")

#define pl_implement_object(className)                                                                       \
    verify_polly_object(className);                                                                          \
    pl_implement_object_funcs(className)

#define pl_implement_derived_object(baseClassName, className)                                                \
    className::className()                                                                                   \
    {                                                                                                        \
    }                                                                                                        \
    className::className(baseClassName::Impl* impl)                                                          \
        : baseClassName(impl)                                                                                \
    {                                                                                                        \
    }

#define pl_implement_derived_object_no_ctor(baseClassName, className)                                        \
    className::className(baseClassName::Impl* impl)                                                          \
        : baseClassName(impl)                                                                                \
    {                                                                                                        \
    }

#define verifyImplAccess                                                                                     \
    if (not impl)                                                                                            \
    {                                                                                                        \
        throw Error(formatString("[{}] Attempting to access an empty object.", __FUNCTION__));               \
    }

#define declareThisImpl                                                                                      \
    const auto impl = this->impl();                                                                          \
    verifyImplAccess

#define declareThisImplNoVerify const auto impl = this->impl();

#define verifyHaveImpl                                                                                       \
    if (not impl())                                                                                          \
    {                                                                                                        \
        throw Error(formatString("[{}] Attempting to access and empty object.", __FUNCTION__));              \
    }

#define declareThisImplAs(type)                                                                              \
    auto* const impl = static_cast<type*>(this->impl());                                                     \
    verifyImplAccess

namespace Polly
{
class Object
{
  public:
    explicit Object() = default;

    deleteCopyAndMove(Object);

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
    verify_polly_object(T);

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
