// Copyright (C) 2025 Cemalettin Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE.

#pragma once

#include <Metal/Metal.hpp>
#include <QuartzCore/CAMetalLayer.hpp>

#define NSStringLiteral(str) NS::String::string(#str, NS::UTF8StringEncoding)

#define NSStringFromC(str) NS::String::string(str, NS::UTF8StringEncoding) // NOLINT(*-macro-usage)

namespace pl
{
class String;
class StringView;

void set_default_ca_metal_layer_props(CA::MetalLayer* layer);

void set_ca_metal_layer_display_sync(CA::MetalLayer* layer, bool enabled);

void remove_file_system_item(const String& path);

void check_ns_error(const NS::Error* error);

MTL::Function* find_mtl_library_function(MTL::Library* library, StringView name);

MTL::Library* create_mtl_library(MTL::Device* device, StringView src_code);
} // namespace pl
