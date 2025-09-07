// Copyright (C) 2025 Cem Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE, or https://polly2d.org.

#include "Polly/Shader.hpp"

#include "Polly/ContentManagement/ContentManager.hpp"
#include "Polly/Game/GameImpl.hpp"
#include "Polly/Graphics/PainterImpl.hpp"

namespace Polly
{
PollyImplementObject(Shader);

Shader::Shader(const StringView assetName)
    : Shader()
{
    auto& content = Game::Impl::instance().contentManager();

    *this = content.loadShader(assetName);
}

Shader Shader::fromSource(StringView name, StringView sourceCode)
{
    auto& painterImpl = *Painter::Impl::instance();

    auto shaderImpl = painterImpl.createUserShader(sourceCode, name);
    shaderImpl->setAssetName(name);

    auto shader = Shader(shaderImpl.release());
    shader.setDebuggingLabel(name);

    return shader;
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

void Shader::setDebuggingLabel(const StringView name)
{
    PollyDeclareThisImpl;
    impl->setDebuggingLabel(name);
}

void Shader::set(const StringView name, const float value)
{
    PollyDeclareThisImpl;
    impl->updateScalarParameter(name, ShaderParameterType::Float, value);
}

void Shader::set(const StringView name, const int value)
{
    PollyDeclareThisImpl;
    impl->updateScalarParameter(name, ShaderParameterType::Int, value);
}

void Shader::set(const StringView name, const bool value)
{
    PollyDeclareThisImpl;
    impl->updateScalarParameter(name, ShaderParameterType::Bool, value);
}

void Shader::set(const StringView name, const Vec2 value)
{
    PollyDeclareThisImpl;
    impl->updateScalarParameter(name, ShaderParameterType::Vec2, value);
}

void Shader::set(const StringView name, const Vec3 value)
{
    PollyDeclareThisImpl;
    impl->updateScalarParameter(name, ShaderParameterType::Vec3, value);
}

void Shader::set(const StringView name, const Vec4 value)
{
    PollyDeclareThisImpl;
    impl->updateScalarParameter(name, ShaderParameterType::Vec4, value);
}

void Shader::set(const StringView name, const Matrix& value)
{
    PollyDeclareThisImpl;
    impl->updateScalarParameter(name, ShaderParameterType::Matrix, value);
}

void Shader::set(const StringView name, const Span<float> values, u32 offset)
{
    PollyDeclareThisImpl;
    impl->updateScalarArrayParameter(name, ShaderParameterType::FloatArray, values, offset);
}

void Shader::set(const StringView name, const Span<int> values, const u32 offset)
{
    PollyDeclareThisImpl;
    impl->updateScalarArrayParameter(name, ShaderParameterType::IntArray, values, offset);
}

void Shader::set(const StringView name, const Span<Vec2> values, const u32 offset)
{
    PollyDeclareThisImpl;
    impl->updateScalarArrayParameter(name, ShaderParameterType::Vec2Array, values, offset);
}

void Shader::set(const StringView name, const Span<Vec3> values, const u32 offset)
{
    PollyDeclareThisImpl;
    impl->updateScalarArrayParameter(name, ShaderParameterType::Vec3Array, values, offset);
}

void Shader::set(const StringView name, const Span<Vec4> values, const u32 offset)
{
    PollyDeclareThisImpl;
    impl->updateScalarArrayParameter(name, ShaderParameterType::Vec4Array, values, offset);
}

void Shader::set(const StringView name, const Span<Matrix> values, const u32 offset)
{
    PollyDeclareThisImpl;
    impl->updateScalarArrayParameter(name, ShaderParameterType::MatrixArray, values, offset);
}

Maybe<float> Shader::floatValue(const StringView name) const
{
    PollyDeclareThisImpl;
    return impl->readParameterData<float>(name, ShaderParameterType::Float);
}

Maybe<int> Shader::intValue(const StringView name) const
{
    PollyDeclareThisImpl;
    return impl->readParameterData<int>(name, ShaderParameterType::Int);
}

Maybe<bool> Shader::boolValue(const StringView name) const
{
    PollyDeclareThisImpl;
    return impl->readParameterData<bool>(name, ShaderParameterType::Bool);
}

Maybe<Vec2> Shader::vec2Value(const StringView name) const
{
    PollyDeclareThisImpl;
    return impl->readParameterData<Vec2>(name, ShaderParameterType::Vec2);
}

Maybe<Vec3> Shader::vec3Value(const StringView name) const
{
    PollyDeclareThisImpl;
    return impl->readParameterData<Vec3>(name, ShaderParameterType::Vec3);
}

Maybe<Vec4> Shader::vec4Value(const StringView name) const
{
    PollyDeclareThisImpl;
    return impl->readParameterData<Vec4>(name, ShaderParameterType::Vec4);
}

Maybe<Matrix> Shader::matrixValue(const StringView name) const
{
    PollyDeclareThisImpl;
    return impl->readParameterData<Matrix>(name, ShaderParameterType::Matrix);
}

bool Shader::hasParameter(const StringView name) const
{
    PollyDeclareThisImpl;
    return impl->findParameter(name) != nullptr;
}
} // namespace Polly