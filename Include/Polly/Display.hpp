// Copyright (C) 2025 Cem Dervis
// This file is part of Polly, a 2D C++ game framework for minimalists.
// For conditions of distribution and use, see copyright notice in LICENSE, or https://polly2d.org.

#pragma once

#include "Polly/List.hpp"
#include "Polly/Maybe.hpp"

namespace Polly
{
/// Defines the pixel format of a display mode.
enum class DisplayFormat
{
    Unknown,
    ABGR1555,
    ABGR4444,
    ABGR8888,
    ARGB1555,
    ARGB4444,
    ARGB8888,
    BGR24,
    BGR565,
    BGRA4444,
    BGRA5551,
    BGRA8888,
    BGRX8888,
    RGB24,
    RGB332,
    RGB565,
    RGBA4444,
    RGBA5551,
    RGBA8888,
    RGBX8888,
    XBGR1555,
    XBGR4444,
    XBGR8888,
    XRGB1555,
    XRGB4444,
    XRGB8888,
};

/// Represents a mode of a display (i.e. "which resolutions does the display
/// support, which formats, which refresh rates?").
struct DisplayMode
{
    /// If known, the format of the mode
    DisplayFormat format;

    /// The width of the mode, in pixels
    u32 width = 0;

    /// The height of the mode, in pixels
    u32 height = 0;

    /// The refresh rate of the mode (Hz)
    float refreshRate = 0.0f;

    /// The DPI scale factor of the mode
    float pixelDensity = 0.0f;

    DefineDefaultEqualityOperations(DisplayMode);
};

/// Defines the fixed orientation of a display.
enum class DisplayOrientation
{
    /// An indeterminate orientation.
    Unknown = 0,

    /// The display is in landscape mode, with the right side up, relative to portrait mode.
    Landscape = 1,

    /// The display is in landscape mode, with the left side up, relative to portrait mode.
    LandscapeFlipped = 2,

    /// The display is in portrait mode.
    Portrait = 3,

    /// The display is in portrait mode, upside down.
    PortraitFlipped = 4,
};

/// Stores information about a connected display.
struct Display
{
    u32                id;
    Maybe<DisplayMode> currentMode;
    List<DisplayMode>  modes;
    DisplayOrientation orientation;
    float              contentScale;
};
} // namespace Polly
