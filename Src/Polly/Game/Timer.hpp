// Copyright (C) 2025 Cem Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE.

#pragma once

#include "Polly/Prerequisites.hpp"
#include <SDL3/SDL.h>

#include <cstddef>

namespace Polly
{
class Timer
{
  public:
    void init()
    {
        _time_point = SDL_GetTicksNS();
    }

    void sleep()
    {
        _time_point += _time_between_frames;

        SDL_DelayPrecise(_time_point - (static_cast<u64>(100 * 1000)));

        while (SDL_GetTicksNS() < _time_point)
            ;
    }

    void setTimeBetweenFrames(u64 frames)
    {
        _time_between_frames = frames;
    }

  private:
    u64 _time_point          = 0;
    u64 _time_between_frames = static_cast<Uint64>((1.0 / 60.0) * 1'000'000'000);
};
} // namespace Polly
