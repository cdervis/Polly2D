// Copyright (C) 2025 Cem Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE.

#include "Polly/Graphics/Metal/MetalUserShader.hpp"

#include "Polly/Graphics/Metal/MetalHelper.hpp"
#include "Polly/Graphics/Metal/MetalPainter.hpp"

namespace Polly
{
MetalUserShader::MetalUserShader(
    Painter::Impl&  painter,
    ShaderType      shaderType,
    StringView      sourceCode,
    String          metalSourceCode,
    ParameterList   parameters,
    UserShaderFlags flags,
    u16             cbufferSize)
    : Impl(painter, shaderType, sourceCode, std::move(parameters), flags, cbufferSize)
{
    auto& metalPainter = static_cast<MetalPainter&>(painter);
    auto* mtlDevice    = metalPainter.mtlDevice();

    // Cast to non-const because NS::String::init() expects void*.
    // This is fine, because it optionally frees the specified buffer.
    // But since we pass false (don't free the buffer), it's a read-only operation.
    const auto metalSrcCodeStr = NS::String::alloc()->init(
        /*pBytes=*/metalSourceCode.data(),
        /*len=*/metalSourceCode.size(),
        /*encoding=*/NS::StringEncoding::UTF8StringEncoding,
        /*freeBuffer=*/false);

    auto opts = NS::TransferPtr(MTL::CompileOptions::alloc()->init());
    opts->setLibraryType(MTL::LibraryTypeExecutable);
    opts->setLanguageVersion(MTL::LanguageVersion1_1);
    opts->setFastMathEnabled(false);
    opts->setOptimizationLevel(MTL::LibraryOptimizationLevelDefault);

    NS::Error* error      = nullptr;
    const auto mtlLibrary = NS::TransferPtr(mtlDevice->newLibrary(metalSrcCodeStr, opts.get(), &error));

    checkNSError(error);

    _mtlFunction = findMtlLibraryFunction(mtlLibrary.get(), "ps_main");

    if (not _mtlFunction)
    {
        throw Polly::Error("Invalid user-shader loaded.");
    }
}

MTL::Function* MetalUserShader::mtlFunction() const
{
    return _mtlFunction;
}

void MetalUserShader::setDebuggingLabel(StringView name)
{
    const auto nameStr = String(name);
    _mtlFunction->setLabel(NSStringFromC(nameStr.cstring()));
}
} // namespace Polly