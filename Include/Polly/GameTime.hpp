// Copyright (C) 2025 Cem Dervis
// This file is part of Polly, a 2D C++ game framework for minimalists.
// For conditions of distribution and use, see copyright notice in LICENSE, or https://polly2d.org.

#pragma once

namespace Polly
{
/// Represents timing information about a running game.
///
/// A GameTime isn't created by the user directly.
/// It's instead obtained as a parameter by the callback function that's attached to Game::update().
class GameTime
{
  public:
    constexpr GameTime()
        : GameTime(0.0, 0.0)
    {
    }

    constexpr explicit GameTime(double elapsed, double total)
        : _elapsed(float(elapsed))
        , _elapsedPrecise(elapsed)
        , _total(total)
    {
    }

    /// Gets the time that has elapsed since the last frame, in fractional seconds.
    constexpr float elapsed() const
    {
        return _elapsed;
    }

    /// Gets the time that has elapsed since the last frame, in fractional seconds, as a double.
    constexpr double elapsedPrecise() const
    {
        return _elapsedPrecise;
    }

    /// Gets the time that has elapsed since the game started running, in fractional seconds
    constexpr double total() const
    {
        return _total;
    }

    DefineDefaultEqualityOperations(GameTime);

  private:
    float  _elapsed;
    double _elapsedPrecise;
    double _total;
};
} // namespace Polly
