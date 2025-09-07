// Copyright (C) 2025 Cem Dervis
// This file is part of Polly, a 2D C++ game framework for minimalists.
// For conditions of distribution and use, see copyright notice in LICENSE, or https://polly2d.org.

#pragma once

#include "Polly/Prerequisites.hpp"

namespace Polly
{
struct Vec2;
struct Vec2i;
struct Vec2ui;
struct Vec3;
struct Vec3i;
struct Vec4;
struct Vec4i;
struct Matrix;
struct Radians;

/// Calculates a version of a 2D vector with all of its components being their
/// absolute values.
Vec2 abs(Vec2 vector);

/// Calculates a version of a 2D vector with all of its components being their
/// absolute values.
Vec2i abs(Vec2i vector);

/// Calculates a version of a 3D vector with all of its components being their
/// absolute values.
Vec3 abs(Vec3 vector);

/// Calculates a version of a 3D vector with all of its components being their
/// absolute values.
Vec3i abs(Vec3i vector);

/// Calculates a version of a 4D vector with all of its components being their
/// absolute values.
Vec4 abs(Vec4 vector);

/// Calculates a version of a 4D vector with all of its components being their
/// absolute values.
Vec4i abs(Vec4i vector);

/// Gets a value indicating whether two 2D vectors are equal within a specific
/// threshold.
///
/// @param lhs The first vector to compare
/// @param rhs The second vector to compare
/// @param threshold The threshold within which both vectors count as equal to one
/// another. The comparison is performed one a per-component basis.
///
/// @return True if lhs and rhs are equal within threshold; false otherwise.
constexpr bool areEqualWithin(Vec2 lhs, Vec2 rhs, float threshold);

/// Gets a value indicating whether two 3D vectors are equal within a specific
/// threshold.
///
/// @param lhs The first vector to compare
/// @param rhs The second vector to compare
/// @param threshold The threshold within which both vectors count as equal to one
/// another. The comparison is performed one a per-component basis.
///
/// @return True if lhs and rhs are equal within threshold; false otherwise.
constexpr bool areEqualWithin(Vec3 lhs, Vec3 rhs, float threshold);

/// Gets a value indicating whether two 4D vectors are equal within a specific
/// threshold.
///
/// @param lhs The first vector to compare
/// @param rhs The second vector to compare
/// @param threshold The threshold within which both vectors count as equal to one
/// another. The comparison is performed one a per-component basis.
///
/// @return True if lhs and rhs are equal within threshold; false otherwise.
constexpr bool areEqualWithin(Vec4 lhs, Vec4 rhs, float threshold);

/// Gets a value indicating whether two matrices are equal within a specific threshold.
///
/// @param lhs The first vector to compare.
/// @param rhs The second vector to compare.
/// @param threshold The threshold within which both vectors count as equal to one
/// another. The comparison is performed one a per-component basis.
///
/// @return True if lhs and rhs are equal within threshold; false otherwise.
constexpr bool areEqualWithin(Matrix lhs, Matrix rhs, float threshold);

/// Rounds a 2D vector's elements down to their nearest integers.
///
/// @param value The vector to round up
Vec2 ceil(Vec2 value);

/// Rounds a 3D vector's elements up to their nearest integers.
///
/// @param value The vector to round up
Vec3 ceil(Vec3 value);

/// Rounds a 4D vector's elements up to their nearest integers.
///
/// @param value The vector to round up
Vec4 ceil(Vec4 value);

/// Clamps a 2D vector into a specific range.
///
/// @param value The value to clamp.
/// @param min The minimum value of the range.
/// @param max The maximum value of the range.
[[nodiscard]]
constexpr Vec2 clamp(Vec2 value, Vec2 min, Vec2 max);

/// Clamps a 2D vector into a specific range.
///
/// @param value The value to clamp.
/// @param min The minimum value of the range.
/// @param max The maximum value of the range.
constexpr Vec2i clamp(Vec2i value, Vec2i min, Vec2i max);

/// Clamps a 3D vector into a specific range.
///
/// @param value The value to clamp
/// @param min The minimum value of the range
/// @param max The maximum value of the range

[[nodiscard]]
constexpr Vec3 clamp(Vec3 value, Vec3 min, Vec3 max);

/// Clamps a 3D vector into a specific range.
///
/// @param value The value to clamp
/// @param min The minimum value of the range
/// @param max The maximum value of the range

constexpr Vec3i clamp(Vec3i value, Vec3i min, Vec3i max);

/// Clamps a 4D vector into a specific range.
///
/// @param value The value to clamp
/// @param min The minimum value of the range
/// @param max The maximum value of the range

[[nodiscard]]
constexpr Vec4 clamp(Vec4 value, Vec4 min, Vec4 max);

/// Clamps a 4D vector into a specific range.
///
/// @param value The value to clamp
/// @param min The minimum value of the range
/// @param max The maximum value of the range
constexpr Vec4i clamp(Vec4i value, Vec4i min, Vec4i max);

/// Calculates the cosine of a 2D vector's elements, specified in radians.
Vec2 cos(Vec2 vector);

/// Calculates the cosine of a 3D vector's elements, specified in radians.
Vec3 cos(Vec3 vector);

/// Calculates the cosine of a 4D vector's elements, specified in radians.
Vec4 cos(Vec4 vector);

/// Calculates the length of a 2D vector.
float length(Vec2 vector);

/// Calculates the length of a 3D vector.
float length(Vec3 vector);

/// Calculates the length of a 4D vector.
float length(Vec4 vector);

/// Calculates the squared length of a 2D vector.
constexpr float lengthSquared(Vec2 vector);

/// Calculates the squared length of a 3D vector.
constexpr float lengthSquared(Vec3 vector);

/// Calculates the squared length of a 4D vector.
constexpr float lengthSquared(Vec4 vector);

/// Calculates the normalized version of a 2D vector.
[[nodiscard]]
Vec2 normalize(Vec2 vector);

/// Calculates the normalized version of a 3D vector.
[[nodiscard]]
Vec3 normalize(Vec3 vector);

/// Calculates the normalized version of a 4D vector.
[[nodiscard]]
Vec4 normalize(Vec4 vector);

/// Calculates the rounded version of a 2D vector.
Vec2 round(Vec2 vector);

/// Calculates the rounded version of a 3D vector.
Vec3 round(Vec3 vector);

/// Calculates the rounded version of a 4D vector.
Vec4 round(Vec4 vector);

/// Calculates the sine of a 2D vector's elements, specified in radians.
Vec2 sin(Vec2 vector);

/// Calculates the sine of a 3D vector's elements, specified in radians.
Vec3 sin(Vec3 vector);

/// Calculates the sine of a 4D vector's elements, specified in radians.
Vec4 sin(Vec4 vector);

/// Calculates the tangent of a 2D vector's elements, specified in radians.
Vec2 tan(Vec2 vector);

/// Calculates the tangent of a 3D vector's elements, specified in radians.
Vec3 tan(Vec3 vector);

/// Calculates the tangent of a 4D vector's elements, specified in radians.
Vec4 tan(Vec4 vector);

/// Calculates the value of base raised to the power exp (2D vector).
Vec2 pow(Vec2 x, Vec2 y);

/// Calculates the value of base raised to the power exp (3D vector).
Vec3 pow(Vec3 base, Vec3 exp);

/// Calculates the value of base raised to the power exp (4D vector).
Vec4 pow(Vec4 x, Vec4 y);

/// Rounds a 2D vector's elements up to their nearest integers.
///
/// @param value The vector to round up
Vec2 floor(Vec2 value);

/// Rounds a 3D vector's elements down to their nearest integers.
///
/// @param value The vector to round down
Vec3 floor(Vec3 value);

/// Rounds a 4D vector's elements down to their nearest integers.
///
/// @param value The vector to round down
Vec4 floor(Vec4 value);

/// Calculates the dot product of two 2D vectors.
constexpr float dot(Vec2 lhs, Vec2 rhs);

/// Calculates the distance between two 2D vectors.
float distance(Vec2 lhs, Vec2 rhs);

/// Calculates the squared distance between two 2D vectors.
constexpr float distanceSquared(Vec2 lhs, Vec2 rhs);

/// Performs a linear interpolation between two 2D vectors.
///
/// @param start The start vector
/// @param end The destination vector
/// @param t The interpolation factor, in the range [0.0 .. 1.0]
constexpr Vec2 lerp(Vec2 start, Vec2 end, float t);

/// Performs a linear interpolation from one 3D vector to another.
///
/// @param end The destination vector
/// @param t The interpolation factor, in the range [0.0 .. 1.0]
constexpr Vec3 lerp(Vec3 start, Vec3 end, float t);

/// Performs a linear interpolation from one 4D vector to another.
///
/// @param start The start vector
/// @param end The destination vector
/// @param t The interpolation factor, in the range [0.0 .. 1.0]
constexpr Vec4 lerp(Vec4 start, Vec4 end, float t);

/// Performs a smoothstep interpolation from one 2D vector to another.
///
/// @param start The start vector
/// @param end The destination vector
/// @param t The interpolation factor, in the range [0.0 .. 1.0]
constexpr Vec2 smoothstep(Vec2 start, Vec2 end, float t);

/// Performs a smoothstep interpolation from one 3D vector to another.
///
/// @param start The start vector
/// @param end The destination vector
/// @param t The interpolation factor, in the range [0.0 .. 1.0]
constexpr Vec3 smoothstep(Vec3 start, Vec3 end, float t);

/// Performs a smoothstep interpolation from one 4D vector to another.
///
/// @param start The start vector
/// @param end The destination vector
/// @param t The interpolation factor, in the range [0.0 .. 1.0]
constexpr Vec4 smoothstep(Vec4 start, Vec4 end, float t);

/// Gets a value indicating whether all components of a 2D vector are exactly
/// equal to zero.
///
/// @param vector The vector to test
constexpr bool isZero(Vec2 vector);

/// Gets a value indicating whether all components of a 2D vector are equal to zero.
///
/// @param vector The vector to test
constexpr bool isZero(Vec2i vector);

/// Gets a value indicating whether all components of a 3D vector are exactly
/// equal to zero.
///
/// @param vector The vector to test
constexpr bool isZero(Vec3 vector);

/// Gets a value indicating whether all components of a 3D vector are exactly
/// equal to zero.
///
/// @param vector The vector to test
constexpr bool isZero(Vec3i vector);

/// Gets a value indicating whether all components of a 4D vector are exactly
/// equal to zero.
///
/// @param vector The vector to test
constexpr bool isZero(Vec4 vector);

/// Gets a value indicating whether all components of a 4D vector are exactly
/// equal to zero.
///
/// @param vector The vector to test
constexpr bool isZero(Vec4i vector);

/// Calculates the smaller of two 2D vectors.
/// The comparison is performed on a per-component basis.
///
/// @param lhs The first vector to compare
/// @param rhs The second vector to compare
constexpr Vec2 min(Vec2 lhs, Vec2 rhs);

/// Calculates the smaller of two 2D vectors.
/// The comparison is performed on a per-component basis.
///
/// @param lhs The first vector to compare
/// @param rhs The second vector to compare
constexpr Vec2i min(Vec2i lhs, Vec2i rhs);

/// Calculates the smaller of two 3D vectors.
/// The comparison is performed on a per-component basis.
///
/// @param lhs The first vector to compare
/// @param rhs The second vector to compare
constexpr Vec3 min(Vec3 lhs, Vec3 rhs);

/// Calculates the smaller of two 3D vectors.
/// The comparison is performed on a per-component basis.
///
/// @param lhs The first vector to compare
/// @param rhs The second vector to compare
constexpr Vec3i min(Vec3i lhs, Vec3i rhs);

/// Calculates the smaller of two 4D vectors.
/// The comparison is performed on a per-component basis.
///
/// @param lhs The first vector to compare
/// @param rhs The second vector to compare
constexpr Vec4 min(Vec4 lhs, Vec4 rhs);

/// Calculates the smaller of two 4D vectors.
/// The comparison is performed on a per-component basis.
///
/// @param lhs The first vector to compare
/// @param rhs The second vector to compare
constexpr Vec4i min(Vec4i lhs, Vec4i rhs);

/// Calculates the larger of two 2D vectors.
/// The comparison is performed on a per-component basis.
///
/// @param lhs The first vector to compare
/// @param rhs The second vector to compare
constexpr Vec2 max(Vec2 lhs, Vec2 rhs);

/// Calculates the larger of two 2D vectors.
/// The comparison is performed on a per-component basis.
///
/// @param lhs The first vector to compare
/// @param rhs The second vector to compare
constexpr Vec2i max(Vec2i lhs, Vec2i rhs);

/// Calculates the larger of two 3D vectors.
/// The comparison is performed on a per-component basis.
///
/// @param lhs The first vector to compare
/// @param rhs The second vector to compare
constexpr Vec3 max(Vec3 lhs, Vec3 rhs);

/// Calculates the larger of two 3D vectors.
/// The comparison is performed on a per-component basis.
///
/// @param lhs The first vector to compare
/// @param rhs The second vector to compare
constexpr Vec3i max(Vec3i lhs, Vec3i rhs);

/// Calculates the larger of two 4D vectors.
/// The comparison is performed on a per-component basis.
///
/// @param lhs The first vector to compare
/// @param rhs The second vector to compare
constexpr Vec4 max(Vec4 lhs, Vec4 rhs);

/// Calculates the larger of two 4D vectors.
/// The comparison is performed on a per-component basis.
///
/// @param lhs The first vector to compare
/// @param rhs The second vector to compare
constexpr Vec4i max(Vec4i lhs, Vec4i rhs);

/// Calculates the normal of a 2D line.
///
/// @param start The start point of the line
/// @param end The end point of the line
Vec2 lineNormal(Vec2 start, Vec2 end);

Vec2 snapToGrid(Vec2 value, Vec2 gridSize);

/// Calculates the dot product of two 3D vectors.
float dot(Vec3 lhs, Vec3 rhs);

/// Calculates the cross product of two 3D vectors.
Vec3 cross(Vec3 lhs, Vec3 rhs);

/// Calculates the distance between two 3D vectors.
///
/// @param lhs The start vector
/// @param rhs The destination vector
float distance(Vec3 lhs, Vec3 rhs);

/// Calculates the squared distance between two 3D vectors.
///
/// @param lhs The start vector
/// @param rhs The destination vector
float distanceSquared(Vec3 lhs, Vec3 rhs);

/// Calculates the dot product of two 4D vectors.
float dot(Vec4 lhs, Vec4 rhs);

/// Calculates the distance between two 4D vectors.
///
/// @param lhs The start vector
/// @param rhs The destination vector
float distance(Vec4 lhs, Vec4 rhs);

/// Calculates the squared distance between two 4D vectors.
///
/// @param lhs The start vector
/// @param rhs The destination vector
float distanceSquared(Vec4 lhs, Vec4 rhs);

/// Transposes a matrix.
///
/// @param matrix The matrix to transpose.
constexpr Polly::Matrix transpose(Matrix matrix);

/// Creates a 2D translation matrix.
///
/// @param translation The resulting translation of the matrix.
constexpr Matrix translate(Vec2 translation);

/// Creates a 2D scaling matrix.
///
/// @param scale The scale factor along the X and Y axes.
constexpr Matrix scale(Vec2 scale);

/// Creates a matrix that rotates around the Z-axis.
///
/// @param radians The rotation amount
Matrix rotate(Radians radians);

/// Adds two 2D vectors.
constexpr Vec2 operator+(Vec2 lhs, Vec2 rhs);

/// Adds two 2D vectors.
constexpr Vec2i operator+(Vec2i lhs, Vec2i rhs);

/// Adds two 3D vectors.
constexpr Vec3 operator+(Vec3 lhs, Vec3 rhs);

/// Adds two 3D vectors.
constexpr Vec3i operator+(Vec3i lhs, Vec3i rhs);

/// Adds two 4D vectors.
constexpr Vec4 operator+(Vec4 lhs, Vec4 rhs);

/// Adds two 4D vectors.
constexpr Vec4i operator+(Vec4i lhs, Vec4i rhs);

/// Adds a 2D vector to another 2D vector.
constexpr Vec2& operator+=(Vec2& lhs, Vec2 rhs);

/// Adds a 2D vector to another 2D vector.
constexpr Vec2i& operator+=(Vec2i& vector, Vec2i rhs);

/// Adds a 3D vector to another 3D vector.
constexpr Vec3& operator+=(Vec3& vector, Vec3 rhs);

/// Adds a 3D vector to another 3D vector.
constexpr Vec3i& operator+=(Vec3i& vector, Vec3i rhs);

/// Adds a 4D vector to another 4D vector.
constexpr Vec4& operator+=(Vec4& vector, Vec4 rhs);

/// Adds a 4D vector to another 4D vector.
constexpr Vec4i& operator+=(Vec4i& vector, Vec4i rhs);

/// Subtracts two 2D vectors.
constexpr Vec2 operator-(Vec2 lhs, Vec2 rhs);

/// Negates a 2D vector.
constexpr Vec2 operator-(Vec2 value);

/// Subtracts two 2D vectors.
constexpr Vec2i operator-(Vec2i lhs, Vec2i rhs);

/// Negates a 2D vector.
constexpr Vec2i operator-(Vec2i value);

/// Subtracts two 3D vectors.
constexpr Vec3 operator-(Vec3 lhs, Vec3 rhs);

/// Negates a 3D vector.
constexpr Vec3 operator-(Vec3 value);

/// Subtracts two 3D vectors.
constexpr Vec3i operator-(Vec3i lhs, Vec3i rhs);

/// Negates a 3D vector.
constexpr Vec3i operator-(Vec3i value);

/// Subtracts two 4D vectors.
constexpr Vec4 operator-(Vec4 lhs, Vec4 rhs);

/// Negates a 4D vector.
constexpr Vec4 operator-(Vec4 value);

/// Subtracts two 4D vectors.
constexpr Vec4i operator-(Vec4i lhs, Vec4i rhs);

/// Negates a 4D vector.
constexpr Vec4i operator-(Vec4i value);

/// Subtracts a 2D vector from another 2D vector.
constexpr Vec2& operator-=(Vec2& lhs, Vec2 rhs);

/// Subtracts a 2D vector from another 2D vector.
constexpr Vec2i& operator-=(Vec2i& vector, Vec2i rhs);

/// Subtracts a 3D vector from another 3D vector.
constexpr Vec3& operator-=(Vec3& vector, Vec3 rhs);

/// Subtracts a 3D vector from another 3D vector.
constexpr Vec3i& operator-=(Vec3i& vector, Vec3i rhs);

/// Subtracts a 4D vector from another 4D vector.
constexpr Vec4& operator-=(Vec4& vector, Vec4 rhs);

/// Subtracts a 4D vector from another 4D vector.
constexpr Vec4i& operator-=(Vec4i& vector, Vec4i rhs);

/// Multiplies two 2D vectors.
constexpr Vec2 operator*(Vec2 lhs, Vec2 rhs);

/// Multiplies a 2D vector by a number.
constexpr Vec2 operator*(Vec2 lhs, float rhs);

/// Multiplies a 2D vector by a number.
constexpr Vec2 operator*(float lhs, Vec2 rhs);

/// Transforms a 2D vector by a matrix.
constexpr Vec2 operator*(Vec2 lhs, Matrix rhs);

/// Multiplies two 3D vectors.
constexpr Vec3 operator*(Vec3 lhs, Vec3 rhs);

/// Multiplies a 3D vector by a number.
constexpr Vec3 operator*(Vec3 lhs, float rhs);

/// Multiplies a 3D vector by a number.
constexpr Vec3 operator*(float lhs, Vec3 rhs);

/// Multiplies two 3D vectors.
constexpr Vec3i operator*(Vec3i lhs, Vec3i rhs);

/// Multiplies a 3D vector by a number.
constexpr Vec3i operator*(Vec3i lhs, int rhs);

/// Multiplies a 3D vector by a number.
constexpr Vec3i operator*(int lhs, Vec3i rhs);

/// Multiplies two 4D vectors.
constexpr Vec4 operator*(Vec4 lhs, Vec4 rhs);

/// Multiplies a 4D vector by a number.
constexpr Vec4 operator*(Vec4 lhs, float rhs);

/// Multiplies a 4D vector by a number.
constexpr Vec4 operator*(float lhs, Vec4 rhs);

/// Multiplies two 4D vectors.
constexpr Vec4i operator*(Vec4i lhs, Vec4i rhs);

/// Multiplies a 4D vector by a number.
constexpr Vec4i operator*(Vec4i lhs, int rhs);

/// Multiplies a 4D vector by a number.
constexpr Vec4i operator*(int lhs, Vec4i rhs);

constexpr Matrix operator*(Matrix lhs, Matrix rhs);

/// Multiplies two 2D vectors.
constexpr Vec2i operator*(Vec2i lhs, Vec2i rhs);

/// Multiplies a 2D vector by a number.
constexpr Vec2i operator*(Vec2i lhs, int rhs);

/// Multiplies a 2D vector by a number.
constexpr Vec2i operator*(int lhs, Vec2i rhs);

/// Scales a 2D vector by another 2D vector.
constexpr Vec2& operator*=(Vec2& lhs, Vec2 rhs);

/// Scales a 2D vector by a number.
constexpr Vec2& operator*=(Vec2& lhs, float rhs);

/// Transforms a 2D vector by a matrix.
constexpr Vec2& operator*=(Vec2& lhs, Matrix rhs);

/// Scales a 2D vector by another 2D vector.
constexpr Vec2i& operator*=(Vec2i& vector, Vec2i rhs);

/// Scales a 2D vector by a number.
constexpr Vec2i& operator*=(Vec2i& vector, int rhs);

/// Scales a 3D vector by another 3D vector.
constexpr Vec3& operator*=(Vec3& vector, Vec3 rhs);

/// Scales a 3D vector by a number.
constexpr Vec3& operator*=(Vec3& vector, float rhs);

/// Scales a 3D vector by another 3D vector.
constexpr Vec3i& operator*=(Vec3i& vector, Vec3i rhs);

/// Scales a 3D vector by a number.
constexpr Vec3i& operator*=(Vec3i& vector, int rhs);

/// Scales a 4D vector by another 4D vector.
constexpr Vec4& operator*=(Vec4& vector, Vec4 rhs);

/// Scales a 4D vector by a number.
constexpr Vec4& operator*=(Vec4& vector, float rhs);

/// Scales a 4D vector by another 4D vector.
constexpr Vec4i& operator*=(Vec4i& vector, Vec4i rhs);

/// Scales a 4D vector by a number.
constexpr Vec4i& operator*=(Vec4i& vector, int rhs);

/// Divides a 2D vector by another 2D vector.
constexpr Vec2 operator/(Vec2 lhs, Vec2 rhs);

/// Divides a 2D vector by a number.
constexpr Vec2 operator/(Vec2 lhs, float rhs);

/// Divides a 2D vector by another 2D vector.
constexpr Vec2i operator/(Vec2i lhs, Vec2i rhs);

/// Divides a 2D vector by a number.
constexpr Vec2i operator/(Vec2i lhs, int rhs);

/// Divides a 3D vector by another 3D vector.
constexpr Vec3 operator/(Vec3 lhs, Vec3 rhs);

/// Divides a 3D vector by a number.
constexpr Vec3 operator/(Vec3 lhs, float rhs);

/// Divides a 3D vector by another 3D vector.
constexpr Vec3i operator/(Vec3i lhs, Vec3i rhs);

/// Divides a 3D vector by a number.
constexpr Vec3i operator/(Vec3i lhs, int rhs);

/// Divides a 4D vector by another 4D vector.
constexpr Vec4 operator/(Vec4 lhs, Vec4 rhs);

/// Divides a 4D vector by a number.
constexpr Vec4 operator/(Vec4 lhs, float rhs);

/// Divides a 4D vector by another 4D vector.
constexpr Vec4i operator/(Vec4i lhs, Vec4i rhs);

/// Divides a 4D vector by a number.
constexpr Vec4i operator/(Vec4i lhs, int rhs);

/// Divides a 2D vector by another 2D vector.
constexpr Vec2& operator/=(Vec2& lhs, Vec2 rhs);

/// Divides a 2D vector by a number.
constexpr Vec2& operator/=(Vec2& lhs, float rhs);

/// Divides a 4D vector by another 4D vector.
constexpr Vec4& operator/=(Vec4& vector, Vec4 rhs);

/// Divides a 4D vector by a number.
constexpr Vec4& operator/=(Vec4& vector, float rhs);

/// Divides a 4D vector by another 4D vector.
constexpr Vec4i& operator/=(Vec4i& vector, Vec4i rhs);

/// Divides a 4D vector by a number.
constexpr Vec4i& operator/=(Vec4i& vector, int rhs);

/// Divides a 2D vector by another 2D vector.
constexpr Vec2i& operator/=(Vec2i& vector, Vec2i rhs);

/// Divides a 2D vector by a number.
constexpr Vec2i& operator/=(Vec2i& vector, int rhs);

/// Divides a 3D vector by another 3D vector.
constexpr Vec3& operator/=(Vec3& vector, Vec3 rhs);

/// Divides a 3D vector by a number.
constexpr Vec3& operator/=(Vec3& vector, float rhs);

/// Divides a 3D vector by another 3D vector.
constexpr Vec3i& operator/=(Vec3i& vector, Vec3i rhs);

/// Divides a 3D vector by a number.
constexpr Vec3i& operator/=(Vec3i& vector, int rhs);
} // namespace Polly

#include "Polly/Details/LinalgOps.inl"