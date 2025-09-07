// Copyright (C) 2025 Cem Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE, or https://polly2d.org.

#include "Polly/Tween.hpp"

#include "Polly/Error.hpp"
#include "Polly/Math.hpp"

#ifdef __GNUC__
#pragma GCC diagnostic ignored "-Wsequence-point"
#endif

namespace Polly
{
void Tweener::resume()
{
    _isRunning = true;
}

void Tweener::update(const float elapsedTime)
{
    if (!_isRunning || _elapsed == _duration)
    {
        return;
    }

    _elapsed += elapsedTime;

    if (_elapsed >= _duration)
    {
        _elapsed  = _duration;
        _position = _from + _change;

        switch (_loopMode)
        {
            case TweenLoopMode::None: break;
            case TweenLoopMode::FrontToBack: reset(); break;
            case TweenLoopMode::BackAndForth: reverse(); break;
        }
    }
    else
    {
        _position = _func(_elapsed, _from, _change, _duration);
    }
}

void Tweener::pause()
{
    _isRunning = false;
}

void Tweener::reset()
{
    _elapsed  = 0;
    _position = _from;
}

void Tweener::restart()
{
    reset();
    resume();
}

void Tweener::reverse()
{
    _elapsed = 0;
    _change  = _from - _position;
    _to      = _from;
    _from    = _position;
}

float Tweener::value() const
{
    return _position;
}

float Tweener::percentage() const
{
    return inverseLerp(_from, _to, _position);
}

bool Tweener::isRunning() const
{
    return _isRunning;
}

bool Tweener::hasEnded() const
{
    return _elapsed == _duration;
}

// NOLINTBEGIN

#ifdef __clang__
#pragma clang diagnostic ignored "-Wunsequenced"
#endif

float Tweener::backEaseIn(float t, float b, float c, float d)
{
    return c * (t /= d) * t * ((1.70158f + 1) * t - 1.70158f) + b;
}

float Tweener::backEaseOut(float t, float b, float c, float d)
{
    return c * ((t = t / d - 1) * t * ((1.70158f + 1) * t + 1.70158f) + 1) + b;
}

float Tweener::backEaseInOut(float t, float b, float c, float d)
{
    float s = 1.70158f;

    if ((t /= d / 2) < 1)
    {
        return c / 2 * (t * t * (((s *= (1.525f)) + 1) * t - s)) + b;
    }

    return c / 2 * ((t -= 2) * t * (((s *= (1.525f)) + 1) * t + s) + 2) + b;
}

float Tweener::bounceEaseOut(float t, float b, float c, float d)
{
    if ((t /= d) < 1 / 2.75)
        return c * (7.5625f * t * t) + b;

    if (t < 2 / 2.75)
    {
        return c * (7.5625f * (t -= (1.5f / 2.75f)) * t + .75f) + b;
    }

    if (t < 2.5 / 2.75)
    {
        return c * (7.5625f * (t -= 2.25f / 2.75f) * t + .9375f) + b;
    }

    return c * (7.5625f * (t -= (2.625f / 2.75f)) * t + .984375f) + b;
}

float Tweener::bounceEaseIn(float t, float b, float c, float d)
{
    return c - bounceEaseOut(d - t, 0, c, d) + b;
}

float Tweener::bounceEaseInOut(float t, float b, float c, float d)
{
    if (t < d / 2)
    {
        return bounceEaseIn(t * 2, 0, c, d) * 0.5f + b;
    }

    return bounceEaseOut(t * 2 - d, 0, c, d) * .5f + c * 0.5f + b;
}

float Tweener::circularEaseIn(float t, float b, float c, float d)
{
    return -c * (Polly::sqrt(1 - (t /= d) * t) - 1) + b;
}

float Tweener::circularEaseOut(float t, float b, float c, float d)
{
    return c * Polly::sqrt(1 - (t = t / d - 1) * t) + b;
}

float Tweener::circularEaseInOut(float t, float b, float c, float d)
{
    if ((t /= d / 2) < 1)
    {
        return -c / 2 * (Polly::sqrt(1 - t * t) - 1) + b;
    }

    return c / 2 * (Polly::sqrt(1 - (t -= 2) * t) + 1) + b;
}

float Tweener::cubicEaseIn(float t, float b, float c, float d)
{
    return c * (t /= d) * t * t + b;
}

float Tweener::cubicEaseOut(float t, float b, float c, float d)
{
    return c * ((t = t / d - 1) * t * t + 1) + b;
}

float Tweener::cubicEaseInOut(float t, float b, float c, float d)
{
    if ((t /= d / 2) < 1)
    {
        return c / 2 * t * t * t + b;
    }

    return c / 2 * ((t -= 2) * t * t + 2) + b;
}

float Tweener::elasticEaseIn(float t, float b, float c, float d)
{
    if (t == 0)
    {
        return b;
    }

    if ((t /= d) == 1)
    {
        return b + c;
    }

    float p = d * 0.3f;
    float s = p / 4;
    return -(c * Polly::pow(2.0f, 10.0f * (t -= 1)) * Polly::sin((t * d - s) * (2 * Polly::pi) / p)) + b;
}

float Tweener::elasticEaseOut(float t, float b, float c, float d)
{
    if (t == 0)
    {
        return b;
    }

    if ((t /= d) == 1)
    {
        return b + c;
    }

    const auto p = d * .3f;
    const auto s = p / 4;

    return c * Polly::pow(2.0f, -10.0f * t) * Polly::sin((t * d - s) * (2 * Polly::pi) / p) + c + b;
}

float Tweener::elasticEaseInOut(float t, float b, float c, float d)
{
    if (t == 0)
    {
        return b;
    }

    if ((t /= d / 2) == 2)
    {
        return b + c;
    }

    const auto p = d * (.3f * 1.5f);
    const auto a = c;
    const auto s = p / 4;

    if (t < 1)
    {
        return -0.5f
                   * (a * Polly::pow(2.0f, 10.0f * (t -= 1)) * Polly::sin((t * d - s) * (2 * Polly::pi) / p))
               + b;
    }

    return a * Polly::pow(2.0f, -10.0f * (t -= 1)) * Polly::sin((t * d - s) * (2 * Polly::pi) / p) * 0.5f
           + c
           + b;
}

float Tweener::exponentialEaseIn(float t, float b, float c, float d)
{
    return t == 0 ? b : c * Polly::pow(2.0f, 10.0f * (t / d - 1)) + b;
}

float Tweener::exponentialEaseOut(float t, float b, float c, float d)
{
    return t == d ? b + c : c * (-Polly::pow(2.0f, -10.0f * t / d) + 1) + b;
}

float Tweener::exponentialEaseInOut(float t, float b, float c, float d)
{
    if (t == 0)
    {
        return b;
    }

    if (t == d)
    {
        return b + c;
    }

    if ((t /= d / 2) < 1)
    {
        return c / 2 * Polly::pow(2.0f, 10.0f * (t - 1)) + b;
    }

    return c / 2 * (-Polly::pow(2.0f, -10.0f * --t) + 2) + b;
}

float Tweener::linear(float t, float b, float c, float d)
{
    return c * t / d + b;
}

float Tweener::quadraticEaseIn(float t, float b, float c, float d)
{
    return c * (t /= d) * t + b;
}

float Tweener::quadraticEaseOut(float t, float b, float c, float d)
{
    return -c * (t /= d) * (t - 2) + b;
}

float Tweener::quadraticEaseInOut(float t, float b, float c, float d)
{
    if ((t /= d / 2) < 1)
    {
        return c / 2 * t * t + b;
    }

    return -c / 2 * ((--t) * (t - 2) - 1) + b;
}

float Tweener::quarticEaseIn(float t, float b, float c, float d)
{
    return c * (t /= d) * t * t * t + b;
}

float Tweener::quarticEaseOut(float t, float b, float c, float d)
{
    return -c * ((t = t / d - 1) * t * t * t - 1) + b;
}

float Tweener::quarticEaseInOut(float t, float b, float c, float d)
{
    if ((t /= d / 2) < 1)
    {
        return c / 2 * t * t * t * t + b;
    }

    return -c / 2 * ((t -= 2) * t * t * t - 2) + b;
}

float Tweener::quinticEaseIn(float t, float b, float c, float d)
{
    return c * (t /= d) * t * t * t * t + b;
}

float Tweener::quinticEaseOut(float t, float b, float c, float d)
{
    return c * ((t = t / d - 1) * t * t * t * t + 1) + b;
}

float Tweener::quinticEaseInOut(float t, float b, float c, float d)
{
    if ((t /= d / 2) < 1)
    {
        return c / 2 * t * t * t * t * t + b;
    }

    return c / 2 * ((t -= 2) * t * t * t * t + 2) + b;
}

float Tweener::sinusoidalEaseIn(float t, float b, float c, float d)
{
    return -c * Polly::cos(t / d * (Polly::pi / 2)) + c + b;
}

float Tweener::sinusoidalEaseOut(float t, float b, float c, float d)
{
    return c * Polly::sin(t / d * (Polly::pi / 2)) + b;
}

float Tweener::sinusoidalEaseInOut(float t, float b, float c, float d)
{
    return -c / 2 * (Polly::cos(Polly::pi * t / d) - 1) + b;
}

// NOLINTEND

TweenerChain::TweenerChain(const Span<Tweener> tweeners)
    : _tweeners(tweeners)
    , _currentTweenerIndex(0)
{
    if (tweeners.isEmpty())
    {
        throw Error("No tweeners specified.");
    }
}

void TweenerChain::resume()
{
    if (hasEnded())
    {
        reset();
    }
    else
    {
        _tweeners[_currentTweenerIndex].resume();
    }
}

void TweenerChain::update(float elapsedTime)
{
    const auto index   = _currentTweenerIndex;
    auto&      tweener = _tweeners[index];
    tweener.update(elapsedTime);

    const auto nextIndex = index + 1;

    if (tweener.hasEnded() && nextIndex < _tweeners.size())
    {
        _currentTweenerIndex = nextIndex;
    }
}

void TweenerChain::pause()
{
    _tweeners[_currentTweenerIndex].pause();
}

void TweenerChain::reset()
{
    for (auto& tweener : _tweeners)
    {
        tweener.reset();
    }

    _currentTweenerIndex = 0;
}

float TweenerChain::value() const
{
    return _tweeners[_currentTweenerIndex].value();
}

bool TweenerChain::hasEnded() const
{
    if (!_currentTweenerIndex)
    {
        return false;
    }

    const auto idx = _currentTweenerIndex;

    return idx == (_tweeners.size() - 1) && _tweeners[idx].hasEnded();
}

const Tweener& TweenerChain::currentTweener() const
{
    return _tweeners[_currentTweenerIndex];
}

u32 TweenerChain::currentTweenerIndex() const
{
    return _currentTweenerIndex;
}
} // namespace Polly