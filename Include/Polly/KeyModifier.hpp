// Copyright (C) 2025 Cem Dervis
// This file is part of Polly, a 2D C++ game framework for minimalists.
// For conditions of distribution and use, see copyright notice in LICENSE, or https://polly2d.org.

#pragma once

#include "Polly/Prerequisites.hpp"

namespace Polly
{
/// Defines a modifier key on a keyboard, such as the shift and control keys.
///
/// Values of this type can be bitwise combined to represent multiple states.
enum class KeyModifier
{
    None          = 0,
    LeftShift     = 0x0001,
    RightShift    = 0x0002,
    Level5        = 0x0004,
    LeftControl   = 0x0040,
    RightControl  = 0x0080,
    LeftAlt       = 0x0100,
    RightAlt      = 0x0200,
    LeftGui       = 0x0400,
    RightGui      = 0x0800,
    Num           = 0x1000,
    Caps          = 0x2000,
    Mode          = 0x4000,
    Scroll        = 0x8000,
    AnyControlKey = LeftControl bitor RightControl,
    AnyShiftKey   = LeftShift bitor RightShift,
    AnyAltKey     = LeftAlt bitor RightAlt,
    AnyGuiKey     = LeftGui bitor RightGui,
};

PollyDefineEnumFlagOperations(KeyModifier);
} // namespace Polly
