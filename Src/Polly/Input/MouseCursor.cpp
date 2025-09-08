// Copyright (C) 2025 Cem Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE, or https://polly2d.org.

#include "Polly/MouseCursor.hpp"

#include "Polly/Input/MouseCursorImpl.hpp"
#include "Polly/UniquePtr.hpp"

namespace Polly
{
PollyImplementObject(MouseCursor);

MouseCursor::MouseCursor(MouseCursorType type)
    : MouseCursor()
{
    setImpl(*this, makeUnique<Impl>(type).release());
}
} // namespace Polly