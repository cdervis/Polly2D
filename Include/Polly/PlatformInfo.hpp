// Copyright (C) 2025 Cemalettin Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE.

#pragma once

#include "Polly/Prerequisites.hpp"

namespace Polly
{
/// Defines a concrete target platform such as Windows, macOS and Linux.
enum class TargetPlatform
{
    Windows,
    macOS,
    iOS,
    Linux,
    Android,
};

namespace Platform
{
/// Gets the current target platform.
[[nodiscard]]
TargetPlatform current();

/// Gets a value indicating whether the current platform counts as a desktop platform.
[[nodiscard]]
bool isDesktop();

/// Gets a value indicating whether the current platform counts as a mobile platform.
[[nodiscard]]
bool isMobile();
} // namespace platform
} // namespace Polly
