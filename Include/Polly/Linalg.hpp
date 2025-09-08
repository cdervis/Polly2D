// Copyright (C) 2025 Cem Dervis
// This file is part of Polly, a minimalistic 2D C++ game framework.
// For conditions of distribution and use, see copyright notice in LICENSE, or https://polly2d.org.

#pragma once

#include "Polly/Prerequisites.hpp"
#include <compare>

namespace Polly
{
/// Represents a floating-point (single-precision) 2D vector.
struct Vec2
{
    /// Creates a 2D vector with all of its components being zero.
    constexpr Vec2();

    /// Creates a 2D vector by splatting a single value to all of its components.
    ///
    /// @param xy The value to set for the X and Y component
    ///
    /// @name From XY
    constexpr explicit Vec2(float xy);

    /// Creates a 2D vector from two separate components.
    ///
    /// @param x The X component
    /// @param y The Y component
    ///
    /// @name From X and Y
    constexpr Vec2(float x, float y);

    DefineDefaultEqualityOperations(Vec2);

    /// The value of the X component
    float x = 0.0f;

    /// The value of the Y component
    float y = 0.0f;
};

/// Represents an integer-based 2D vector.
struct Vec2i
{
    /// Creates a 2D vector with all of its components being zero.
    constexpr Vec2i();

    /// Creates a 2D vector by splatting a single value to all of its components.
    ///
    /// @param xy The value to set for the X and Y component
    constexpr explicit Vec2i(int xy);

    /// Creates a 2D vector from two separate components.
    ///
    /// @param x The X component
    /// @param y The Y component
    constexpr Vec2i(int x, int y);

    constexpr explicit Vec2i(const Vec2& vector); // NOLINT

    DefineDefaultEqualityOperations(Vec2i);

    /// The value of the X component
    int x = 0;

    /// The value of the Y component
    int y = 0;
};

/// Represents an unsigned integer-based 2D vector.
struct Vec2ui
{
    /// Creates a 2D vector with all of its components being zero.
    constexpr Vec2ui();

    /// Creates a 2D vector by splatting a single value to all of its components.
    ///
    /// @param xy The value to set for the X and Y component
    constexpr explicit Vec2ui(unsigned int xy);

    /// Creates a 2D vector from two separate components.
    ///
    /// @param x The X component
    /// @param y The Y component
    constexpr Vec2ui(unsigned int x, unsigned int y);

    DefineDefaultEqualityOperations(Vec2ui);

    /// The value of the X component
    unsigned int x;

    /// The value of the Y component
    unsigned int y;
};

/// Represents a floating-point (single-precision) 3D vector.
struct Vec3
{
    /// Creates a 3D vector with all of its components being zero.
    constexpr Vec3();

    /// Creates a 3D vector by splatting a single value to all of its components.
    ///
    /// @param xyz The value to set for the X, Y and Z component
    constexpr explicit Vec3(float xyz);

    /// Creates a 3D vector from two separate components.
    ///
    /// @param x The X component
    /// @param y The Y component
    /// @param z The Z component
    constexpr Vec3(float x, float y, float z);

    DefineDefaultEqualityOperations(Vec3);

    /// The value of the X component
    float x = 0.0f;

    /// The value of the Y component
    float y = 0.0f;

    /// The value of the Z component
    float z = 0.0f;
};

/// Represents an integer-based 3D vector.
struct Vec3i
{
    /// Creates a 3D vector with all of its components being zero.
    constexpr Vec3i();

    /// Creates a 3D vector by splatting a single value to all of its components.
    ///
    /// @param xyz The value to set for the X, Y and Z component
    constexpr explicit Vec3i(int xyz);

    /// Creates a 3D vector from two separate components.
    ///
    /// @param x The X component
    /// @param y The Y component
    /// @param z The Z component
    constexpr Vec3i(int x, int y, int z);

    DefineDefaultEqualityOperations(Vec3i);

    /// The value of the X component
    int x = 0;

    /// The value of the Y component
    int y = 0;

    /// The value of the Z component
    int z = 0;
};

/// Represents a floating-point (single-precision) 4D vector.
struct Vec4
{
    /// Creates a 4D vector with all of its components being zero.
    constexpr Vec4();

    /// Creates a 4D vector by splatting a single value to all of its components.
    ///
    /// @param xyzw The value to set for the X, Y, Z and W component
    constexpr explicit Vec4(float xyzw);

    /// Creates a 4D vector from two separate components.
    ///
    /// @param x The X component
    /// @param y The Y component
    /// @param z The Z component
    /// @param w The W component
    constexpr Vec4(float x, float y, float z, float w);

    /// Creates a 4D vector from two 2D vectors.
    ///
    /// @param xy The 2D vector that represents the X and Y components
    /// @param zw The 2D vector that represents the Z and W components
    constexpr Vec4(Vec2 xy, Vec2 zw);

    /// Creates a 4D vector from a 2D vector and two numbers.
    ///
    /// @param xy The 2D vector that represents the X and Y components
    /// @param z The Z component
    /// @param w The W component
    constexpr Vec4(Vec2 xy, float z, float w);

    /// Creates a 4D vector from a 3D vector and a number.
    ///
    /// @param xyz The 3D vector that represents the X, Y and Z components
    /// @param w The W component
    constexpr Vec4(Vec3 xyz, float w);

    DefineDefaultEqualityOperations(Vec4);

    /// The value of the X component
    float x = 0.0f;

    /// The value of the Y component
    float y = 0.0f;

    /// The value of the Z component
    float z = 0.0f;

    /// The value of the W component
    float w = 0.0f;
};

/// Represents an integer-based 4D vector.
struct Vec4i
{
    /// Creates a 4D vector with all of its components being zero.
    constexpr Vec4i();

    /// Creates a 4D vector by splatting a single value to all of its components.
    ///
    /// @param xyzw The value to set for the X, Y, Z and W component
    constexpr explicit Vec4i(int xyzw);

    /// Creates a 4D vector from two separate components.
    ///
    /// @param x The X component
    /// @param y The Y component
    /// @param z The Z component
    /// @param w The W component
    constexpr Vec4i(int x, int y, int z, int w);

    /// Creates a 4D vector from two 2D vectors.
    ///
    /// @param xy The 2D vector that represents the X and Y components
    /// @param zw The 2D vector that represents the Z and W components
    constexpr Vec4i(Vec2i xy, Vec2i zw);

    /// Creates a 4D vector from a 2D vector and two numbers.
    ///
    /// @param xy The 2D vector that represents the X and Y components
    /// @param z The Z component
    /// @param w The W component
    constexpr Vec4i(Vec2i xy, int z, int w);

    /// Creates a 4D vector from a 3D vector and a number.
    ///
    /// @param xyz The 3D vector that represents the X, Y and Z components
    /// @param w The W component
    constexpr Vec4i(Vec3i xyz, int w);

    DefineDefaultEqualityOperations(Vec4i);

    /// The value of the X component
    int x = 0;

    /// The value of the Y component
    int y = 0;

    /// The value of the Z component
    int z = 0;

    /// The value of the W component
    int w = 0;
};

/// Represents a floating-point (single-precision) 4x4, row-major matrix.
struct Matrix
{
    /// Creates an identity matrix.
    constexpr Matrix();

    /// @name From rows
    constexpr Matrix(Vec4 row1, Vec4 row2, Vec4 row3, Vec4 row4);

    /// @name From single value
    constexpr explicit Matrix(float diagonalValue);

    const float* data() const;

    float* begin();

    const float* begin() const;

    const float* cbegin() const;

    float* end();

    const float* end() const;

    const float* cend() const;

    DefineDefaultEqualityOperations(Matrix);

    Vec4 row1;
    Vec4 row2;
    Vec4 row3;
    Vec4 row4;
};
} // namespace Polly

#include "Polly/Details/Linalg.inl"
