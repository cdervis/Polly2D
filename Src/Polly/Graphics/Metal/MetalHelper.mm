// Copyright (C) 2025 Cemalettin Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE.

//#pragma clang diagnostic ignored "-Wcast-function-type-mismatch"

#include "Polly/Graphics/Metal/MetalHelper.hpp"

#import <QuartzCore/CAMetalLayer.h>
#import <Foundation/NSFileManager.h>

#include <TargetConditionals.h>
#include "Polly/Logging.hpp"
#include "Polly/Error.hpp"

void Polly::setDefaultCaMetalLayerProps( CA::MetalLayer* layer )
{
    CAMetalLayer* objCLayer = (CAMetalLayer*)layer;
    objCLayer.opaque        = true;
}

void Polly::setCaMetalLayerDisplaySync( CA::MetalLayer* layer, bool enabled )
{
#if not TARGET_OS_IOS
    CAMetalLayer* objCLayer      = (CAMetalLayer*)layer;
    objCLayer.displaySyncEnabled = enabled;
#endif
}

void Polly::removeFileSystemItem(const String& path)
{
    NSString* ns = [NSString stringWithUTF8String:path.cstring()];
    [[NSFileManager defaultManager] removeItemAtPath:ns error:nil];
}

void Polly::checkNSError(const NS::Error* error)
{
    if (error)
    {
        throw Error(formatString("NSError occurred: {}", error->description()->utf8String()));
    }
}

MTL::Function* Polly::findMtlLibraryFunction(MTL::Library* library, StringView name)
{
    // Obj-C uses string pooling and MTL::Library::newFunction seems to compare
    // string pointers instead of actual values. So we have to look up the
    // NS::String first.

    const NS::String* nameStr       = nil;
    const auto        names         = library->functionNames();
    const auto        functionCount = names->count();

    for (NS::UInteger i = 0; i < functionCount; ++i) {
        auto* obj = names->object(i);
        auto* str = obj->description();

        if (name == str->utf8String()) {
            nameStr = str;
            break;
        }
    }

    MTL::Function* func = library->newFunction(nameStr);

    assume(func);

    return func;
}

MTL::Library* Polly::createMtlLibrary(MTL::Device* device, StringView srcCode)
{
    const auto srcCodeStr = String(srcCode);
    NS::Error* error      = nullptr;

    auto* lib = device->newLibrary(
        NS::String::string(srcCode.cstring(), NS::UTF8StringEncoding),
        nullptr,
        &error);

    Polly::checkNSError(error);

    return lib;
}
