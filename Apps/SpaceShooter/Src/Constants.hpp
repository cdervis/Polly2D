#pragma once

#include <Polly.hpp>

#if NDEBUG
constexpr auto sSkipSplashScreen = false;
#else
constexpr auto sSkipSplashScreen = true;
#endif

constexpr auto sViewSize                 = Vec2(128, 172);
constexpr auto sWindowSizeMultiplier     = 3.0;
constexpr auto sEnemyTimeToShootInterval = FloatInterval(2.0f, 3.0f);
constexpr auto sSplashLogoEntryTweener   = Tweener(0.0f, 1.0f, 3.0f, Tweener::quadraticEaseIn);
constexpr auto sSplashLogoExitTweener    = Tweener(1.0f, 0.0f, 2.5f, Tweener::quadraticEaseOut);
constexpr auto sSplashBackgroundTweener  = Tweener(1.0f, 0.0f, 1.5f, Tweener::quadraticEaseInOut);
