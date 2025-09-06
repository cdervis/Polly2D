// Copyright (C) 2025 Cem Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE, or https://polly2d.org.

#pragma once

#include "Polly/Pair.hpp"
#include "Polly/PlatformInfo.hpp"
#include "Polly/StringView.hpp"

#include "Polly/Graphics/OpenGL/glad.h"

#ifdef min
#undef min
#endif

#ifdef max
#undef max
#endif

namespace Polly
{
static constexpr auto minimumRequiredOpenGLVersionMajor = 3;
static constexpr auto minimumRequiredOpenGLVersionMinor = 3;

void verifyOpenGLState();
} // namespace Polly
