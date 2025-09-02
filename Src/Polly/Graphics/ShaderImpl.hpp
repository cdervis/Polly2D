// Copyright (C) 2025 Cem Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE.

#pragma once

#include "Polly/Graphics/GraphicsResource.hpp"
#include "Polly/Graphics/ShaderParameter.hpp"
#include "Polly/List.hpp"
#include "Polly/Logging.hpp"
#include "Polly/Painter.hpp"
#include "Polly/Shader.hpp"
#include "Polly/SortedSet.hpp"
#include "Polly/Span.hpp"

namespace Polly
{
class Image;
enum class ShaderType;

enum class UserShaderFlags
{
    None             = 0,
    UsesSystemValues = 1,
};

defineEnumFlagOperations(UserShaderFlags);

class Shader::Impl : public GraphicsResource
{
    friend Painter::Impl;

  public:
    using ParameterList     = List<ShaderParameter, 8>;
    using ParameterPtrsList = List<ShaderParameter*, 8>;

  protected:
    explicit Impl(
        Painter::Impl&  painterImpl,
        ShaderType      shaderType,
        StringView      sourceCode,
        ParameterList   parameters,
        UserShaderFlags flags,
        u16             cbufferSize);

  public:
    DeleteCopyAndMove(Impl);

    ~Impl() noexcept override;

    ShaderType shaderType() const;

    static String shaderParameterTypeString(ShaderParameterType type);

    static void verifyParameterRead(
        StringView          parameterName,
        ShaderParameterType dstType,
        ShaderParameterType srcType);

    static void verifyParameterWrite(
        StringView          parameterName,
        ShaderParameterType dstType,
        ShaderParameterType srcType);

    template<typename T>
    [[nodiscard]]
    Maybe<T> readParameterData(StringView name, ShaderParameterType type) const
    {
        if (const auto param = findParameter(name))
        {
            verifyParameterRead(name, param->type, type);
            return *reinterpret_cast<const T*>(_cbufferData.data() + param->offset);
        }

        return Maybe<T>();
    }

    template<typename T>
    void updateScalarParameter(StringView name, ShaderParameterType type, const T& srcData)
    {
        const auto* param = findParameter(name);

        if (not param)
        {
            return;
        }

        assume(param->name == name);

        verifyParameterWrite(name, param->type, type);
        const auto dstData = reinterpret_cast<T*>(_cbufferData.data() + param->offset);

        if (*dstData == srcData)
        {
            return;
        }

        // Notify the device with our current data.
        notifyPainterBeforeParamChanged();

        *dstData = srcData;
        _dirtyScalarParameters.add(param);

        // Notify the device with the fresh cbuffer data.
        notifyPainterAfterParamChanged();

        logVerbose("Updated scalar parameter '{}'", name);
    }

    template<typename T>
    void updateScalarArrayParameter(StringView name, ShaderParameterType type, Span<T> srcData, u32 offset)
    {
        const auto* param = findParameter(name);

        if (not param)
        {
            return;
        }

        verifyParameterWrite(name, param->type, type);

        const auto srcCount = srcData.size();

        if (srcCount + offset > param->arraySize)
        {
            if (offset > 0)
            {
                throw Error(formatString(
                    "The number of specified values and offset ({} + {} = {}) exceeds the "
                    "parameter's array size (= {}).",
                    srcCount,
                    offset,
                    srcCount + offset,
                    param->arraySize));
            }

            throw Error(formatString(
                "The number of specified values (= {}) exceeds the parameter's "
                "array size (= {}).",
                srcCount,
                param->arraySize));
        }

        constexpr auto incrementPerElement = ShaderParameter::arrayElementBaseAlignment;

        notifyPainterBeforeParamChanged();

        auto* dstData = _cbufferData.data() + param->offset;
        dstData += (static_cast<size_t>(offset * incrementPerElement));

        for (u32 i = 0; i < srcCount; ++i, dstData += incrementPerElement)
        {
            *reinterpret_cast<T*>(dstData) = srcData[i];
        }

        _dirtyScalarParameters.add(param);
        notifyPainterAfterParamChanged();

        logVerbose("Updated scalar array parameter '{}'", name);
    }

    ShaderParameter* findParameter(StringView name);

    const ShaderParameter* findParameter(StringView name) const;

    bool hasDirtyParameters() const;

    const SortedSet<const ShaderParameter*>& dirtyScalarParameters() const;

    void clearDirtyScalarParameters();

    const u8* cbufferData() const;

    u32 cbufferSize() const;

    Span<ShaderParameter> allParameters() const;

    UserShaderFlags flags() const;

    bool usesSystemValues() const
    {
        return hasFlag(_flags, UserShaderFlags::UsesSystemValues);
    }

  private:
    void setDefaultParameterValues();

    void notifyPainterBeforeParamChanged();

    void notifyPainterAfterParamChanged();

    Polly::ShaderType _shaderType;

#ifndef NDEBUG
    String _sourceCode;
#endif

    List<u8, 64>                      _cbufferData;
    ParameterList                     _parameters;
    SortedSet<const ShaderParameter*> _dirtyScalarParameters;
    UserShaderFlags                   _flags   = UserShaderFlags::None;
    bool                              _isInUse = false;
};
} // namespace Polly
