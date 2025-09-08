// Copyright (C) 2025 Cem Dervis
// This file is part of Polly, a minimalistic 2D C++ game framework.
// For conditions of distribution and use, see copyright notice in LICENSE, or https://polly2d.org.

#pragma once

#include "Polly/Prerequisites.hpp"

namespace Polly::Details
{
using MainFunction = int (*)(int, char**);

[[nodiscard]]
int runGame(int a, char* b[], MainFunction c, void* d);
} // namespace Polly::Details
