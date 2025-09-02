// Copyright (C) 2025 Cem Dervis
// This file is part of Polly, a 2D C++ game framework for minimalists.
// For conditions of distribution and use, see copyright notice in LICENSE, or https://polly2d.org.

#pragma once

namespace Polly
{
constexpr Tweener::Tweener()
    : _position(0)
    , _from(0)
    , _change(0)
    , _duration(0)
    , _loopMode()
    , _elapsed(0)
    , _isRunning(true)
    , _to(0)
    , _func(nullptr)
{
}

constexpr Tweener::Tweener(float from, float to, float duration, TweeningFunction* tweening_function)
    : _position(from)
    , _from(from)
    , _change(to - from)
    , _duration(duration)
    , _loopMode()
    , _elapsed(0)
    , _isRunning(true)
    , _to(to)
    , _func(tweening_function)
{
}
} // namespace Polly