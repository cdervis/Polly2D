// Copyright (C) 2025 Cem Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE, or https://polly2d.org.

#include "Polly/Shader.hpp"
#include "Polly/ContentManagement/ContentManager.hpp"
#include "Polly/Game/GameImpl.hpp"

namespace Polly
{
PollyImplementObject(Shader);

Shader::Shader(StringView assetName)
    : Shader()
{
    auto& content = Game::Impl::instance().contentManager();

    *this = content.loadShader(assetName);
}

StringView Shader::assetName() const
{
    PollyDeclareThisImpl;
    return impl->assetName();
}

StringView Shader::debuggingLabel() const
{
    PollyDeclareThisImpl;
    return impl->debuggingLabel();
}

void Shader::setDebuggingLabel(StringView name)
{
    PollyDeclareThisImpl;
    impl->setDebuggingLabel(name);
}

void Shader::set(StringView name, float value)
{
    PollyDeclareThisImpl;
    impl->updateScalarParameter(name, ShaderParameterType::Float, value);
}

void Shader::set(StringView name, int value)
{
    PollyDeclareThisImpl;
    impl->updateScalarParameter(name, ShaderParameterType::Int, value);
}

void Shader::set(StringView name, bool value)
{
    PollyDeclareThisImpl;
    impl->updateScalarParameter(name, ShaderParameterType::Bool, value);
}

void Shader::set(StringView name, Vec2 value)
{
    PollyDeclareThisImpl;
    impl->updateScalarParameter(name, ShaderParameterType::Vec2, value);
}

void Shader::set(StringView name, Vec3 value)
{
    PollyDeclareThisImpl;
    impl->updateScalarParameter(name, ShaderParameterType::Vec3, value);
}

void Shader::set(StringView name, Vec4 value)
{
    PollyDeclareThisImpl;
    impl->updateScalarParameter(name, ShaderParameterType::Vec4, value);
}

void Shader::set(StringView name, const Matrix& value)
{
    PollyDeclareThisImpl;
    impl->updateScalarParameter(name, ShaderParameterType::Matrix, value);
}

void Shader::set(StringView name, Span<float> values, u32 offset)
{
    PollyDeclareThisImpl;
    impl->updateScalarArrayParameter(name, ShaderParameterType::FloatArray, values, offset);
}

void Shader::set(StringView name, Span<int> values, u32 offset)
{
    PollyDeclareThisImpl;
    impl->updateScalarArrayParameter(name, ShaderParameterType::IntArray, values, offset);
}

void Shader::set(StringView name, Span<Vec2> values, u32 offset)
{
    PollyDeclareThisImpl;
    impl->updateScalarArrayParameter(name, ShaderParameterType::Vec2Array, values, offset);
}

void Shader::set(StringView name, Span<Vec3> values, u32 offset)
{
    PollyDeclareThisImpl;
    impl->updateScalarArrayParameter(name, ShaderParameterType::Vec3Array, values, offset);
}

void Shader::set(StringView name, Span<Vec4> values, u32 offset)
{
    PollyDeclareThisImpl;
    impl->updateScalarArrayParameter(name, ShaderParameterType::Vec4Array, values, offset);
}

void Shader::set(StringView name, Span<Matrix> values, u32 offset)
{
    PollyDeclareThisImpl;
    impl->updateScalarArrayParameter(name, ShaderParameterType::MatrixArray, values, offset);
}

Maybe<float> Shader::floatValue(StringView name) const
{
    PollyDeclareThisImpl;
    return impl->readParameterData<float>(name, ShaderParameterType::Float);
}

Maybe<int> Shader::intValue(StringView name) const
{
    PollyDeclareThisImpl;
    return impl->readParameterData<int>(name, ShaderParameterType::Int);
}

Maybe<bool> Shader::boolValue(StringView name) const
{
    PollyDeclareThisImpl;
    return impl->readParameterData<bool>(name, ShaderParameterType::Bool);
}

Maybe<Vec2> Shader::vec2Value(StringView name) const
{
    PollyDeclareThisImpl;
    return impl->readParameterData<Vec2>(name, ShaderParameterType::Vec2);
}

Maybe<Vec3> Shader::vec3Value(StringView name) const
{
    PollyDeclareThisImpl;
    return impl->readParameterData<Vec3>(name, ShaderParameterType::Vec3);
}

Maybe<Vec4> Shader::vec4Value(StringView name) const
{
    PollyDeclareThisImpl;
    return impl->readParameterData<Vec4>(name, ShaderParameterType::Vec4);
}

Maybe<Matrix> Shader::matrixValue(StringView name) const
{
    PollyDeclareThisImpl;
    return impl->readParameterData<Matrix>(name, ShaderParameterType::Matrix);
}

bool Shader::hasParameter(StringView name) const
{
    PollyDeclareThisImpl;
    return impl->findParameter(name) != nullptr;
}
} // namespace Polly