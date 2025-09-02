// Copyright (C) 2025 Cem Dervis
// This file is part of Polly, a 2D C++ game framework for minimalists.
// For conditions of distribution and use, see copyright notice in LICENSE, or https://polly2d.org.

// This file contains random, public utility functions.

#pragma once

#include "Polly/Prerequisites.hpp"
#include "Polly/String.hpp"

namespace Polly
{
/// Gets a user-friendly display string for a byte size.
///
/// For example, the size "1000000" would result in "1 MB".
///
/// @param size The size to display, in bytes
String bytesDisplayString(u64 size);
} // namespace Polly
