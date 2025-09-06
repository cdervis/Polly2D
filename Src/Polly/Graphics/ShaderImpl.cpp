// Copyright (C) 2025 Cem Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE, or https://polly2d.org.

#include "Polly/Graphics/ShaderImpl.hpp"

#include "Polly/Algorithm.hpp"
#include "Polly/Graphics/PainterImpl.hpp"
#include "Polly/Logging.hpp"
#include "Polly/Util.hpp"

namespace Polly
{
String Shader::Impl::shaderParameterTypeString(const ShaderParameterType type)
{
    switch (type)
    {
        case ShaderParameterType::Float: return "float";
        case ShaderParameterType::Int: return "int";
        case ShaderParameterType::Bool: return "bool";
        case ShaderParameterType::Vec2: return "Vec2";
        case ShaderParameterType::Vec3: return "Vec3";
        case ShaderParameterType::Vec4: return "Vec4";
        case ShaderParameterType::Matrix: return "Matrix";
        case ShaderParameterType::FloatArray: return "float[]";
        case ShaderParameterType::IntArray: return "int[]";
        case ShaderParameterType::BoolArray: return "bool[]";
        case ShaderParameterType::Vec2Array: return "Vec2[]";
        case ShaderParameterType::Vec3Array: return "Vec3[]";
        case ShaderParameterType::Vec4Array: return "Vec4[]";
        case ShaderParameterType::MatrixArray: return "Matrix[]";
    }

    return {};
}

Shader::Impl::Impl(
    Painter::Impl&        painterImpl,
    const ShaderType      shaderType,
    const StringView      sourceCode,
    ParameterList         parameters,
    const UserShaderFlags flags,
    const u16             cbufferSize)
    : GraphicsResource(painterImpl, GraphicsResourceType::Shader)
    , _shaderType(shaderType)
#ifndef NDEBUG
    , _sourceCode(sourceCode)
#endif
    , _parameters(std::move(parameters))
    , _flags(flags)
{
    _cbufferData.resize(cbufferSize);

    // Because we use binary search to look up parameters, sort them here once.
    sort(_parameters, [](const auto& lhs, const auto& rhs) { return lhs.name < rhs.name; });

    for (auto& param : _parameters)
    {
        _dirtyScalarParameters.add(&param);
    }

    setDefaultParameterValues();
}

Shader::Impl::~Impl() noexcept
{
    logVerbose("~Shader::Impl({})", debuggingLabel());
    painter().notifyUserShaderDestroyed(*this);
}

ShaderType Shader::Impl::shaderType() const
{
    return _shaderType;
}

void Shader::Impl::verifyParameterRead(
    StringView                parameterName,
    const ShaderParameterType dstType,
    const ShaderParameterType srcType)
{
    if (dstType != srcType)
    {
        throw Error(formatString(
            "Attempting to read value of parameter '{}' (type '{}') as "
            "a value of type '{}'.",
            parameterName,
            shaderParameterTypeString(srcType),
            shaderParameterTypeString(dstType)));
    }
}

void Shader::Impl::verifyParameterWrite(
    StringView                parameterName,
    const ShaderParameterType dstType,
    const ShaderParameterType srcType)
{
    if (dstType != srcType)
    {
        throw Error(formatString(
            "Attempting to set value of parameter '{}' (type '{}') to "
            "a value of type '{}'.",
            parameterName,
            shaderParameterTypeString(dstType),
            shaderParameterTypeString(srcType)));
    }
}

ShaderParameter* Shader::Impl::findParameter(const StringView name)
{
    return binaryFind(_parameters, name);
}

const ShaderParameter* Shader::Impl::findParameter(const StringView name) const
{
    return binaryFind(_parameters, name);
}

bool Shader::Impl::hasDirtyParameters() const
{
    return not _dirtyScalarParameters.isEmpty();
}

const SortedSet<const ShaderParameter*>& Shader::Impl::dirtyScalarParameters() const
{
    return _dirtyScalarParameters;
}

void Shader::Impl::clearDirtyScalarParameters()
{
    _dirtyScalarParameters.clear();
}

bool Shader::Impl::hasCBufferData() const
{
    return !_cbufferData.isEmpty();
}

const u8* Shader::Impl::cbufferData() const
{
    return _cbufferData.data();
}

u32 Shader::Impl::cbufferSize() const
{
    return _cbufferData.size();
}

Span<ShaderParameter> Shader::Impl::allParameters() const
{
    return _parameters;
}

UserShaderFlags Shader::Impl::flags() const
{
    return _flags;
}

void Shader::Impl::setDefaultParameterValues()
{
    for (auto& param : _parameters)
    {
        switch (param.type)
        {
            case ShaderParameterType::Float:
                updateScalarParameter(param.name, param.type, param.defaultValue.getOr(0.0f));
                break;
            case ShaderParameterType::Int:
                updateScalarParameter(param.name, param.type, param.defaultValue.getOr(0));
                break;
            case ShaderParameterType::Bool:
                updateScalarParameter(param.name, param.type, param.defaultValue.getOr(false));
                break;
            case ShaderParameterType::Vec2:
                updateScalarParameter(param.name, param.type, param.defaultValue.getOr(Vec2()));
                break;
            case ShaderParameterType::Vec3:
                updateScalarParameter(param.name, param.type, param.defaultValue.getOr(Vec3()));
                break;
            case ShaderParameterType::Vec4:
                updateScalarParameter(param.name, param.type, param.defaultValue.getOr(Vec4()));
                break;
            case ShaderParameterType::Matrix:
                updateScalarParameter(param.name, param.type, param.defaultValue.getOr(Matrix()));
                break;
            default: break;
        }
    }
}

void Shader::Impl::notifyPainterBeforeParamChanged()
{
    if (_isInUse)
    {
        painter().notifyShaderParamAboutToChangeWhileBound(*this);
    }
}

void Shader::Impl::notifyPainterAfterParamChanged()
{
    if (_isInUse)
    {
        painter().notifyShaderParamHasChangedWhileBound(*this);
    }
}
} // namespace Polly
