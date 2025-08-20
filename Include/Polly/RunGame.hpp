// Copyright (C) 2025 Cemalettin Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE.

#pragma once

#include "Polly/Prerequisites.hpp"

namespace Polly::Details
{
using MainFunction = int (*)(int, char**);

[[nodiscard]]
int runGame(int a, char* b[], MainFunction c, void* d);
} // namespace Polly::Details
