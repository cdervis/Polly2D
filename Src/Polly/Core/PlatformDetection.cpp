// Copyright (C) 2025 Cem Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE.

// ReSharper disable CppDFAUnreachableCode
// ReSharper disable CppDFAConstantConditions

#include "Polly/PlatformInfo.hpp"

#include "Polly/Core/PlatformDetection.hpp"

#ifdef __APPLE__
#include <TargetConditionals.h>
#endif

namespace Polly
{
TargetPlatform Platform::current()
{
#if polly_platform_windows
    return TargetPlatform::Windows;
#elif polly_platform_macos
    return TargetPlatform::macOS;
#elif polly_platform_linux
    return TargetPlatform::Linux;
#elif polly_platform_android
    return TargetPlatform::Android;
#elif polly_platform_ios
    return TargetPlatform::iOS;
#else
#error "Unhandled target platform"
#endif
}

bool Platform::isDesktop()
{
    const auto platform = current();

    return platform == TargetPlatform::Windows
           or platform == TargetPlatform::macOS
           or platform == TargetPlatform::Linux;
}

// ReSharper disable once CppDFAConstantFunctionResult
bool Platform::isMobile()
{
    return current() == TargetPlatform::Android;
}
} // namespace Polly