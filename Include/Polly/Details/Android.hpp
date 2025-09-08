// Copyright (C) 2025 Cem Dervis
// This file is part of Polly, a minimalistic 2D C++ game framework.
// For conditions of distribution and use, see copyright notice in LICENSE, or https://polly2d.org.

#pragma once

#include <android/asset_manager_jni.h>

namespace Polly::Details
{
void set_android_asset_manager(void* asset_manager);
}

#define POLLY_ANDROID_DEFINE_JNI_FUNCTIONS(companyName, gameName)                                            \
    extern "C" {                                                                                             \
    JNIEXPORT void JNICALL Java_com_##companyName##_##gameName##_MainActivity_setAssetManager(               \
        JNIEnv* env,                                                                                         \
        jclass  obj,                                                                                         \
        jobject assetManager)                                                                                \
    {                                                                                                        \
        const auto nativeAssetManager = AAssetManager_fromJava(env, assetManager);                           \
        pl::Details::set_android_asset_manager(nativeAssetManager);                                          \
    }                                                                                                        \
    }
