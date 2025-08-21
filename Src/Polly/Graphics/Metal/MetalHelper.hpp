// Copyright (C) 2025 Cemalettin Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE.

#pragma once

#include <Metal/Metal.hpp>
#include <QuartzCore/CAMetalLayer.hpp>

#define NSStringLiteral(str) NS::String::string(#str, NS::UTF8StringEncoding)

#define NSStringFromC(str) NS::String::string(str, NS::UTF8StringEncoding) // NOLINT(*-macro-usage)

namespace Polly
{
class String;
class StringView;

void setDefaultCaMetalLayerProps(CA::MetalLayer* layer);

void setCaMetalLayerDisplaySync(CA::MetalLayer* layer, bool enabled);

void removeFileSystemItem(const String& path);

void checkNSError(const NS::Error* error);

MTL::Function* findMtlLibraryFunction(MTL::Library* library, StringView name);

MTL::Library* createMtlLibrary(MTL::Device* device, StringView src_code);
} // namespace Polly
