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

void pl::set_default_ca_metal_layer_props( CA::MetalLayer* layer )
{
    CAMetalLayer* objc_layer = (CAMetalLayer*)layer;
    objc_layer.opaque        = true;
}

void pl::set_ca_metal_layer_display_sync( CA::MetalLayer* layer, bool enabled )
{
#if TARGET_OS_IOS
    LogInfo( "Ignoring VSync configuration on iOS" );
#else
    CAMetalLayer* objc_layer      = (CAMetalLayer*)layer;
    objc_layer.displaySyncEnabled = enabled;
#endif
}

void pl::remove_file_system_item(const String& path)
{
    NSString* ns = [NSString stringWithUTF8String:path.cstring()];
    [[NSFileManager defaultManager] removeItemAtPath:ns error:nil];
}

void pl::check_ns_error(const NS::Error* error)
{
    if (error)
    {
        throw pl::Error(formatString("NSError occurred: {}", error->description()->utf8String()));
    }
}

MTL::Function* pl::find_mtl_library_function(MTL::Library* library, StringView name)
{
    // Obj-C uses string pooling and MTL::Library::newFunction seems to compare
    // string pointers instead of actual values. So we have to look up the
    // NS::String first.

    const NS::String* name_str       = nil;
    const auto        names         = library->functionNames();
    const auto        function_count = names->count();

    for (NS::UInteger i = 0; i < function_count; ++i) {
        auto* obj = names->object(i);
        auto* str = obj->description();

        if (name == str->utf8String()) {
            name_str = str;
            break;
        }
    }

    MTL::Function* func = library->newFunction(name_str);

    assume(func);

    return func;
}

MTL::Library* pl::create_mtl_library(MTL::Device* device, StringView src_code)
{
    const auto src_code_str = String(src_code);
    NS::Error* error         = nullptr;

    auto* lib = device->newLibrary(
        NS::String::string(src_code_str.cstring(), NS::UTF8StringEncoding),
        nullptr,
        &error);

    pl::check_ns_error(error);

    return lib;
}
