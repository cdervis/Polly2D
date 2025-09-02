// Copyright (C) 2025 Cem Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE.

#pragma once

#include "Polly/List.hpp"
#include "Polly/Prerequisites.hpp"
#include "Polly/Seconds.hpp"

namespace Polly
{
enum class TweenLoopMode
{
    None         = 0,
    FrontToBack  = 1,
    BackAndForth = 2,
};

/// Represents a function used to animate properties in a tweener.
///
/// Built-in functions are part of the Tween class, such as Tweener::linear().
using TweeningFunction = float(float elapsed, float from, float change, float duration);

/// Represents a tween object.
///
/// Tweening is used to animate values using specific functions (curves) over time.
/// A tweener is typically created as part of an object, such as a Player class.
/// The player is then able to obtain the updated (animated) property value.
///
/// Example:
///
/// @code
/// // Create a tweener that linearly goes from 0 to 100 during 2 seconds.
/// auto tweener = Tweener(0.0f, 100.0f, 2.0f, Tweener::Linear);
///
/// const auto valueBefore = tweener.value();
///
/// tweener.update(dt);
///
/// const auto valueAfter = tweener.value();
/// @endcode
///
/// @tip For more details, see https://en.wikipedia.org/wiki/Inbetweening
class Tweener
{
  public:
    /// Creates an empty, invalid tweener.
    ///
    /// @note Attempting to update an invalid tweener will throw an exception.
    constexpr Tweener();

    /// Creates a tweener.
    ///
    /// @param from The starting value
    /// @param to The target value (inclusive)
    /// @param duration The duration of the tweener to go from start to finish
    /// @param tweening_function The tweening function to use
    constexpr Tweener(float from, float to, Seconds duration, TweeningFunction* tweening_function);

    /// Resumes the tweener's playback.
    ///
    /// The tweener resumes from its current value.
    ///
    /// @note Resuming an already running tweener has no effect.
    void resume();

    /// Updates the tweener's playback.
    ///
    /// @note If the tweener is not running or has reached its target,
    ///       this has no effect. In that case, call start()
    ///       or restart() to resume playback.
    void update(float elapsedTime);

    /// Stops the tweener's playback.
    ///
    /// The tweener will not reset its value to its starting value.
    /// To do so, call reset().
    ///
    /// @note Pausing an already paused tweener has no effect.
    void pause();

    /// Resets the tweener's playback to its starting value.
    void reset();

    /// Restarts the tweener's playback.
    ///
    /// This is a shortcut to calling reset(), followed by resume().
    void restart();

    /// Reverses the tweener's direction.
    void reverse();

    /// Gets the current value of the property.
    float value() const;

    /// Gets a percentage value indicating how far the tweener has progressed, in
    /// the range `[0.0 .. 1.0]`.
    float percentage() const;

    /// Gets a value indicating whether the tweener is running.
    ///
    /// @note This does not mean that the tweener is updating itself automatically.
    ///         It instead means that when the tweener is not running (i.e. no resume()
    ///         was called), that calls to update() will be ignored. It is still your
    ///         responsibility to update the tweener manually using update().
    bool isRunning() const;

    /// Gets a value indicating whether the tweener has reached its target value.
    bool hasEnded() const;

    // Built-in tweening functions:

    static float backEaseIn(float t, float b, float c, float d);
    static float backEaseOut(float t, float b, float c, float d);
    static float backEaseInOut(float t, float b, float c, float d);
    static float bounceEaseOut(float t, float b, float c, float d);
    static float bounceEaseIn(float t, float b, float c, float d);
    static float bounceEaseInOut(float t, float b, float c, float d);
    static float circularEaseIn(float t, float b, float c, float d);
    static float circularEaseOut(float t, float b, float c, float d);
    static float circularEaseInOut(float t, float b, float c, float d);
    static float cubicEaseIn(float t, float b, float c, float d);
    static float cubicEaseOut(float t, float b, float c, float d);
    static float cubicEaseInOut(float t, float b, float c, float d);
    static float elasticEaseIn(float t, float b, float c, float d);
    static float elasticEaseOut(float t, float b, float c, float d);
    static float elasticEaseInOut(float t, float b, float c, float d);
    static float exponentialEaseIn(float t, float b, float c, float d);
    static float exponentialEaseOut(float t, float b, float c, float d);
    static float exponentialEaseInOut(float t, float b, float c, float d);
    static float linear(float t, float b, float c, float d);
    static float quadraticEaseIn(float t, float b, float c, float d);
    static float quadraticEaseOut(float t, float b, float c, float d);
    static float quadraticEaseInOut(float t, float b, float c, float d);
    static float quarticEaseIn(float t, float b, float c, float d);
    static float quarticEaseOut(float t, float b, float c, float d);
    static float quarticEaseInOut(float t, float b, float c, float d);
    static float quinticEaseIn(float t, float b, float c, float d);
    static float quinticEaseOut(float t, float b, float c, float d);
    static float quinticEaseInOut(float t, float b, float c, float d);
    static float sinusoidalEaseIn(float t, float b, float c, float d);
    static float sinusoidalEaseOut(float t, float b, float c, float d);
    static float sinusoidalEaseInOut(float t, float b, float c, float d);

  private:
    float             _position;
    float             _from;
    float             _change;
    float             _duration;
    TweenLoopMode     _loopMode;
    float             _elapsed;
    bool              _isRunning;
    float             _to;
    TweeningFunction* _func;
};

class TweenerChain
{
  public:
    // ReSharper disable once CppNonExplicitConvertingConstructor
    TweenerChain(Span<Tweener> tweeners);

    void resume();

    void update(float elapsedTime);

    void pause();

    void reset();

    float value() const;

    bool hasEnded() const;

    const Tweener& currentTweener() const;

    u32 currentTweenerIndex() const;

  private:
    List<Tweener, 4> _tweeners;
    u32              _currentTweenerIndex;
};
} // namespace Polly

#include "Polly/Details/Tween.inl"