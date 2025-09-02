// Copyright (C) 2025 Cem Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE, or https://polly2d.org.

#include "Polly/Core/Object.hpp"

#include "Polly/Logging.hpp"

namespace Polly
{
void Object::addRef()
{
    ++_refCount;
}

u64 Object::release()
{
    assume(_refCount > 0);

    const auto newRefCount = --_refCount;

    if (newRefCount == 0)
    {
        delete this;
    }

    return newRefCount;
}

u64 Object::refCount() const
{
    return _refCount;
}
} // namespace Polly