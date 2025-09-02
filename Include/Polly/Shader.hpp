// Copyright (C) 2025 Cem Dervis
// This file is part of Polly, a 2D C++ game framework for minimalists.
// For conditions of distribution and use, see copyright notice in LICENSE, or https://polly2d.org.

#pragma once

#include "Polly/Linalg.hpp"
#include "Polly/Prerequisites.hpp"
#include "Polly/Span.hpp"
#include "Polly/StringView.hpp"

namespace Polly
{
/// Defines the type of a shader.
enum class ShaderType
{
    /// A shader that acts on sprites
    Sprite,

    /// A shader that acts on polygons produced by methods such as
    /// drawLine() and fillRectangle().
    Polygon,
};

/// Represents a user-programmable pixel shader.
///
/// A Shader is used to perform custom shading on things that are
/// drawn on screen.

class Shader
{
    PollyObject(Shader);

  public:
    /// Lazily loads a Shader object from the storage.
    ///
    /// If the shader asset is already loaded, this object will point to it
    /// and increment its reference count.
    ///
    /// @param assetName The name of the shader in the asset storage.
    ///
    /// @throw Error If the asset does not exist or could not be read or loaded.
    explicit Shader(StringView assetName);

    StringView assetName() const;

    /// Gets the debuggable name of the shader.
    StringView debuggingLabel() const;

    /// Sets the debuggable name of the shader.
    ///
    /// The name additionally appears in graphics debuggers.
    void setDebuggingLabel(StringView name);

    /// Sets the value of a float parameter.
    void set(StringView name, float value);

    /// Sets the value of a signed 32-bit integer parameter.
    void set(StringView name, int value);

    /// Sets the value of a boolean parameter.
    void set(StringView name, bool value);

    /// Sets the value of a 2D vector parameter.
    void set(StringView name, Vec2 value);

    /// Sets the value of a 3D vector parameter.
    void set(StringView name, Vec3 value);

    /// Sets the value of a 4D vector parameter.
    void set(StringView name, Vec4 value);

    /// Sets the value of a matrix parameter.
    void set(StringView name, const Matrix& value);

    /// Sets the value of a float array parameter.
    void set(StringView name, Span<float> values, u32 offset = 0);

    /// Sets the value of a 32-bit integer array parameter.
    void set(StringView name, Span<int> values, u32 offset = 0);

    /// Sets the value of a 2D vector array parameter.
    void set(StringView name, Span<Vec2> values, u32 offset = 0);

    /// Sets the value of a 3D vector array parameter.
    void set(StringView name, Span<Vec3> values, u32 offset = 0);

    /// Sets the value of a 4D vector array parameter.
    void set(StringView name, Span<Vec4> values, u32 offset = 0);

    /// Sets the value of a matrix array parameter.
    void set(StringView name, Span<Matrix> values, u32 offset = 0);

    /// Gets the float value of a parameter.
    Maybe<float> floatValue(StringView name) const;

    /// Gets the signed 32-bit integer value of a parameter.
    Maybe<int> intValue(StringView name) const;

    /// Gets the boolean value of a parameter.
    Maybe<bool> boolValue(StringView name) const;

    /// Gets the 2D vector value of a parameter.
    Maybe<Vec2> vec2Value(StringView name) const;

    /// Gets the 3D vector value of a parameter.
    Maybe<Vec3> vec3Value(StringView name) const;

    /// Gets the 4D vector value of a parameter.
    Maybe<Vec4> vec4Value(StringView name) const;

    /// Gets the matrix value of a parameter.
    Maybe<Matrix> matrixValue(StringView name) const;

    /// Gets a value indicating whether the shader contains a parameter with a specific name.
    bool hasParameter(StringView name) const;
};
} // namespace Polly