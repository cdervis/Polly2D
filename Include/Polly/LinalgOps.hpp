// Copyright (C) 2025 Cemalettin Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE.

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
Vec2 abs(const Vec2& vector);

/// Calculates a version of a 2D vector with all of its components being their
/// absolute values.
Vec2i abs(const Vec2i& vector);

/// Calculates a version of a 3D vector with all of its components being their
/// absolute values.
Vec3 abs(const Vec3& vector);

/// Calculates a version of a 3D vector with all of its components being their
/// absolute values.
Vec3i abs(const Vec3i& vector);

/// Calculates a version of a 4D vector with all of its components being their
/// absolute values.
Vec4 abs(const Vec4& vector);

/// Calculates a version of a 4D vector with all of its components being their
/// absolute values.
Vec4i abs(const Vec4i& vector);

/// Gets a value indicating whether two 2D vectors are equal within a specific
/// threshold.
///
/// @param lhs The first vector to compare
/// @param rhs The second vector to compare
/// @param threshold The threshold within which both vectors count as equal to one
/// another. The comparison is performed one a per-component basis.
///
/// @return True if `lhs` and `rhs` are equal within `threshold`; false otherwise.
constexpr bool areEqualWithin(const Vec2& lhs, const Vec2& rhs, float threshold);

/// Gets a value indicating whether two 3D vectors are equal within a specific
/// threshold.
///
/// @param lhs The first vector to compare
/// @param rhs The second vector to compare
/// @param threshold The threshold within which both vectors count as equal to one
/// another. The comparison is performed one a per-component basis.
///
/// @return True if `lhs` and `rhs` are equal within `threshold`; false otherwise.
constexpr bool areEqualWithin(const Vec3& lhs, const Vec3& rhs, float threshold);

/// Gets a value indicating whether two 4D vectors are equal within a specific
/// threshold.
///
/// @param lhs The first vector to compare
/// @param rhs The second vector to compare
/// @param threshold The threshold within which both vectors count as equal to one
/// another. The comparison is performed one a per-component basis.
///
/// @return True if `lhs` and `rhs` are equal within `threshold`; false otherwise.
constexpr bool areEqualWithin(const Vec4& lhs, const Vec4& rhs, float threshold);

/// Gets a value indicating whether two matrices are equal within a specific threshold.
///
/// @param lhs The first vector to compare.
/// @param rhs The second vector to compare.
/// @param threshold The threshold within which both vectors count as equal to one
/// another. The comparison is performed one a per-component basis.
///
/// @return True if `lhs` and `rhs` are equal within `threshold`; false otherwise.
constexpr bool areEqualWithin(const Matrix& lhs, const Matrix& rhs, float threshold);

/// Rounds a 2D vector's elements down to their nearest integers.
///
/// @param value The vector to round up
Vec2 ceil(const Vec2& value);

/// Rounds a 3D vector's elements up to their nearest integers.
///
/// @param value The vector to round up
Vec3 ceil(const Vec3& value);

/// Rounds a 4D vector's elements up to their nearest integers.
///
/// @param value The vector to round up
Vec4 ceil(const Vec4& value);

/// Clamps a 2D vector into a specific range.
///
/// @param value The value to clamp.
/// @param min The minimum value of the range.
/// @param max The maximum value of the range.
[[nodiscard]]
constexpr Vec2 clamp(const Vec2& value, const Vec2& min, const Vec2& max);

/// Clamps a 2D vector into a specific range.
///
/// @param value The value to clamp.
/// @param min The minimum value of the range.
/// @param max The maximum value of the range.
constexpr Vec2i clamp(const Vec2i& value, const Vec2i& min, const Vec2i& max);

/// Clamps a 3D vector into a specific range.
///
/// @param value The value to clamp
/// @param min The minimum value of the range
/// @param max The maximum value of the range

[[nodiscard]]
constexpr Vec3 clamp(const Vec3& value, const Vec3& min, const Vec3& max);

/// Clamps a 3D vector into a specific range.
///
/// @param value The value to clamp
/// @param min The minimum value of the range
/// @param max The maximum value of the range

constexpr Vec3i clamp(const Vec3i& value, const Vec3i& min, const Vec3i& max);

/// Clamps a 4D vector into a specific range.
///
/// @param value The value to clamp
/// @param min The minimum value of the range
/// @param max The maximum value of the range

[[nodiscard]]
constexpr Vec4 clamp(const Vec4& value, const Vec4& min, const Vec4& max);

/// Clamps a 4D vector into a specific range.
///
/// @param value The value to clamp
/// @param min The minimum value of the range
/// @param max The maximum value of the range
constexpr Vec4i clamp(const Vec4i& value, const Vec4i& min, const Vec4i& max);

/// Calculates the cosine of a 2D vector's elements, specified in radians.
Vec2 cos(const Vec2& vector);

/// Calculates the cosine of a 3D vector's elements, specified in radians.
Vec3 cos(const Vec3& vector);

/// Calculates the cosine of a 4D vector's elements, specified in radians.
Vec4 cos(const Vec4& vector);

/// Calculates the length of a 2D vector.
float length(const Vec2& vector);

/// Calculates the length of a 3D vector.
float length(const Vec3& vector);

/// Calculates the length of a 4D vector.
float length(const Vec4& vector);

/// Calculates the squared length of a 2D vector.
constexpr float lengthSquared(const Vec2& vector);

/// Calculates the squared length of a 3D vector.
constexpr float lengthSquared(const Vec3& vector);

/// Calculates the squared length of a 4D vector.
constexpr float lengthSquared(const Vec4& vector);

/// Calculates the normalized version of a 2D vector.
[[nodiscard]]
Vec2 normalize(const Vec2& vector);

/// Calculates the normalized version of a 3D vector.
[[nodiscard]]
Vec3 normalize(const Vec3& vector);

/// Calculates the normalized version of a 4D vector.
[[nodiscard]]
Vec4 normalize(const Vec4& vector);

/// Calculates the rounded version of a 2D vector.
Vec2 round(const Vec2& vector);

/// Calculates the rounded version of a 3D vector.
Vec3 round(const Vec3& vector);

/// Calculates the rounded version of a 4D vector.
Vec4 round(const Vec4& vector);

/// Calculates the sine of a 2D vector's elements, specified in radians.
Vec2 sin(const Vec2& vector);

/// Calculates the sine of a 3D vector's elements, specified in radians.
Vec3 sin(const Vec3& vector);

/// Calculates the sine of a 4D vector's elements, specified in radians.
Vec4 sin(const Vec4& vector);

/// Calculates the tangent of a 2D vector's elements, specified in radians.
Vec2 tan(const Vec2& vector);

/// Calculates the tangent of a 3D vector's elements, specified in radians.
Vec3 tan(const Vec3& vector);

/// Calculates the tangent of a 4D vector's elements, specified in radians.
Vec4 tan(const Vec4& vector);

/// Calculates the value of `base` raised to the power `exp` (2D vector).
Vec2 pow(const Vec2& x, const Vec2& y);

/// Calculates the value of `base` raised to the power `exp` (3D vector).
Vec3 pow(const Vec3& base, const Vec3& exp);

/// Calculates the value of `base` raised to the power `exp` (4D vector).
Vec4 pow(const Vec4& x, const Vec4& y);

/// Rounds a 2D vector's elements up to their nearest integers.
///
/// @param value The vector to round up
Vec2 floor(const Vec2& value);

/// Rounds a 3D vector's elements down to their nearest integers.
///
/// @param value The vector to round down
Vec3 floor(const Vec3& value);

/// Rounds a 4D vector's elements down to their nearest integers.
///
/// @param value The vector to round down
Vec4 floor(const Vec4& value);

/// Calculates the dot product of two 2D vectors.
constexpr float dot(const Vec2& lhs, const Vec2& rhs);

/// Calculates the distance between two 2D vectors.
float distance(const Vec2& lhs, const Vec2& rhs);

/// Calculates the squared distance between two 2D vectors.
constexpr float distanceSquared(const Vec2& lhs, const Vec2& rhs);

/// Performs a linear interpolation between two 2D vectors.
///
/// @param start The start vector
/// @param end The destination vector
/// @param t The interpolation factor, in the range `[0.0 .. 1.0]`
constexpr Vec2 lerp(const Vec2& start, const Vec2& end, float t);

/// Performs a linear interpolation from one 3D vector to another.
///
/// @param end The destination vector
/// @param t The interpolation factor, in the range `[0.0 .. 1.0]`
constexpr Vec3 lerp(const Vec3& start, const Vec3& end, float t);

/// Performs a linear interpolation from one 4D vector to another.
///
/// @param start The start vector
/// @param end The destination vector
/// @param t The interpolation factor, in the range `[0.0 .. 1.0]`
constexpr Vec4 lerp(const Vec4& start, const Vec4& end, float t);

/// Performs a smoothstep interpolation from one 2D vector to another.
///
/// @param start The start vector
/// @param end The destination vector
/// @param t The interpolation factor, in the range `[0.0 .. 1.0]`
constexpr Vec2 smoothstep(const Vec2& start, const Vec2& end, float t);

/// Performs a smoothstep interpolation from one 3D vector to another.
///
/// @param start The start vector
/// @param end The destination vector
/// @param t The interpolation factor, in the range `[0.0 .. 1.0]`
constexpr Vec3 smoothstep(const Vec3& start, const Vec3& end, float t);

/// Performs a smoothstep interpolation from one 4D vector to another.
///
/// @param start The start vector
/// @param end The destination vector
/// @param t The interpolation factor, in the range `[0.0 .. 1.0]`
constexpr Vec4 smoothstep(const Vec4& start, const Vec4& end, float t);

/// Gets a value indicating whether all components of a 2D vector are exactly
/// equal to zero.
///
/// @param vector The vector to test
constexpr bool isZero(const Vec2& vector);

/// Gets a value indicating whether all components of a 2D vector are equal to zero.
///
/// @param vector The vector to test
constexpr bool isZero(const Vec2i& vector);

/// Gets a value indicating whether all components of a 3D vector are exactly
/// equal to zero.
///
/// @param vector The vector to test
constexpr bool isZero(const Vec3& vector);

/// Gets a value indicating whether all components of a 3D vector are exactly
/// equal to zero.
///
/// @param vector The vector to test
constexpr bool isZero(const Vec3i& vector);

/// Gets a value indicating whether all components of a 4D vector are exactly
/// equal to zero.
///
/// @param vector The vector to test
constexpr bool isZero(const Vec4& vector);

/// Gets a value indicating whether all components of a 4D vector are exactly
/// equal to zero.
///
/// @param vector The vector to test
constexpr bool isZero(const Vec4i& vector);

/// Calculates the smaller of two 2D vectors.
/// The comparison is performed on a per-component basis.
///
/// @param lhs The first vector to compare
/// @param rhs The second vector to compare
///
/// @code
/// Vec2 smallerVector = Min( Vec2( 1, -2 ), Vec2( -2, 3 ) );
/// // => Vec2( -2, -2 )
/// @endcode
constexpr Vec2 min(const Vec2& lhs, const Vec2& rhs);

/// Calculates the smaller of two 2D vectors.
/// The comparison is performed on a per-component basis.
///
/// @param lhs The first vector to compare
/// @param rhs The second vector to compare
///
/// @code
/// auto smallerVector = Min( Vec2i( 1, -2 ), Vec2i( -2, 3 ) );
/// // => Vec2i( -2, -2 )
/// @endcode
constexpr Vec2i min(const Vec2i& lhs, const Vec2i& rhs);

/// Calculates the smaller of two 3D vectors.
/// The comparison is performed on a per-component basis.
///
/// @param lhs The first vector to compare
/// @param rhs The second vector to compare
///
/// @code
/// auto smallerVector = Min( Vec3( 1, -2, 3 ), Vec3( -2, 3, 4 ) );
/// // => Vec3( -2, -2, 3 )
/// @endcode
constexpr Vec3 min(const Vec3& lhs, const Vec3& rhs);

/// Calculates the smaller of two 3D vectors.
/// The comparison is performed on a per-component basis.
///
/// @param lhs The first vector to compare
/// @param rhs The second vector to compare
///
/// @code
/// auto smallerVector = Min( Vec3i( 1, -2, 3 ), Vec3i( -2, 3, 4 ) );
/// // => Vec3i( -2, -2, 3 )
/// @endcode
constexpr Vec3i min(const Vec3i& lhs, const Vec3i& rhs);

/// Calculates the smaller of two 4D vectors.
/// The comparison is performed on a per-component basis.
///
/// @param lhs The first vector to compare
/// @param rhs The second vector to compare
///
/// @code
/// auto smallerVector = Min( Vec4( 1, -2, 3, -1 ), Vec4( -2, 3, 4, 5 ) );
/// // => Vec4( -2, -2, 3, -1 )
/// @endcode
constexpr Vec4 min(const Vec4& lhs, const Vec4& rhs);

/// Calculates the smaller of two 4D vectors.
/// The comparison is performed on a per-component basis.
///
/// @param lhs The first vector to compare
/// @param rhs The second vector to compare
///
/// @code
/// auto smallerVector = Min( Vec4i( 1, -2, 3, -1 ), Vec4i( -2, 3, 4, 5 ) );
/// // => Vec4i( -2, -2, 3, -1 )
/// @endcode
constexpr Vec4i min(const Vec4i& lhs, const Vec4i& rhs);

/// Calculates the larger of two 2D vectors.
/// The comparison is performed on a per-component basis.
///
/// @param lhs The first vector to compare
/// @param rhs The second vector to compare
///
/// @code
/// auto largerVector = Max( Vec2( 1, -2 ), Vec2( -2, 3 ) );
/// // => Vec2( 1, 3 )
/// @endcode
constexpr Vec2 max(const Vec2& lhs, const Vec2& rhs);

/// Calculates the larger of two 2D vectors.
/// The comparison is performed on a per-component basis.
///
/// @param lhs The first vector to compare
/// @param rhs The second vector to compare
///
/// @code
/// auto largerVector = Max( Vec2i( 1, -2 ), Vec2i( -2, 3 ) );
/// // => Vec2i( 1, 3 )
/// @endcode
constexpr Vec2i max(const Vec2i& lhs, const Vec2i& rhs);

/// Calculates the larger of two 3D vectors.
/// The comparison is performed on a per-component basis.
///
/// @param lhs The first vector to compare
/// @param rhs The second vector to compare
///
/// @code
/// auto largerVector = Max( Vec3( 1, -2, 3 ), Vec3( -2, 3, 4 ) );
/// // => Vec3(1, 3, 4)
/// @endcode
constexpr Vec3 max(const Vec3& lhs, const Vec3& rhs);

/// Calculates the larger of two 3D vectors.
/// The comparison is performed on a per-component basis.
///
/// @param lhs The first vector to compare
/// @param rhs The second vector to compare
///
/// @code
/// auto largerVector = Max( Vec3i( 1, -2, 3 ), Vec3i( -2, 3, 4 ) );
/// // => Vec3i( 1, 3, 4 ).
/// @endcode
constexpr Vec3i max(const Vec3i& lhs, const Vec3i& rhs);

/// Calculates the larger of two 4D vectors.
/// The comparison is performed on a per-component basis.
///
/// @param lhs The first vector to compare
/// @param rhs The second vector to compare
///
/// @code
/// auto largerVector = Max( Vec4( 1, -2, 3, -1 ), Vec4( -2, 3, 4, 5 ) );
/// // => Vec4( 1, 3, 4, 5 )
/// @endcode
constexpr Vec4 max(const Vec4& lhs, const Vec4& rhs);

/// Calculates the larger of two 4D vectors.
/// The comparison is performed on a per-component basis.
///
/// @param lhs The first vector to compare
/// @param rhs The second vector to compare
///
/// @code
/// auto largerVector = Max( Vec4i( 1, -2, 3, -1 ), Vec4i( -2, 3, 4, 5 ) );
/// // => Vec4i( 1, 3, 4, 5 )
/// @endcode
constexpr Vec4i max(const Vec4i& lhs, const Vec4i& rhs);

/// Calculates the normal of a 2D line.
///
/// @param start The start point of the line
/// @param end The end point of the line
Vec2 lineNormal(const Vec2& start, const Vec2& end);

Vec2 snapToGrid(Vec2 value, Vec2 gridSize);

/// Calculates the dot product of two 3D vectors.
float dot(const Vec3& lhs, const Vec3& rhs);

/// Calculates the cross product of two 3D vectors.
Vec3 cross(const Vec3& lhs, const Vec3& rhs);

/// Calculates the distance between two 3D vectors.
///
/// @param lhs The start vector
/// @param rhs The destination vector
float distance(const Vec3& lhs, const Vec3& rhs);

/// Calculates the squared distance between two 3D vectors.
///
/// @param lhs The start vector
/// @param rhs The destination vector
float distanceSquared(const Vec3& lhs, const Vec3& rhs);

/// Calculates the dot product of two 4D vectors.
float dot(const Vec4& lhs, const Vec4& rhs);

/// Calculates the distance between two 4D vectors.
///
/// @param lhs The start vector
/// @param rhs The destination vector
float distance(const Vec4& lhs, const Vec4& rhs);

/// Calculates the squared distance between two 4D vectors.
///
/// @param lhs The start vector
/// @param rhs The destination vector
float distanceSquared(const Vec4& lhs, const Vec4& rhs);

/// Transposes a matrix.
///
/// @param matrix The matrix to transpose.
constexpr Polly::Matrix transpose(const Matrix& matrix);

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
constexpr Vec2 operator+(const Vec2& lhs, const Vec2& rhs);

/// Adds two 2D vectors.
constexpr Vec2i operator+(const Vec2i& lhs, const Vec2i& rhs);

/// Adds two 3D vectors.
constexpr Vec3 operator+(const Vec3& lhs, const Vec3& rhs);

/// Adds two 3D vectors.
constexpr Vec3i operator+(const Vec3i& lhs, const Vec3i& rhs);

/// Adds two 4D vectors.
constexpr Vec4 operator+(const Vec4& lhs, const Vec4& rhs);

/// Adds two 4D vectors.
constexpr Vec4i operator+(const Vec4i& lhs, const Vec4i& rhs);

/// Adds a 2D vector to another 2D vector.
constexpr Vec2& operator+=(Vec2& lhs, const Vec2& rhs);

/// Adds a 2D vector to another 2D vector.
constexpr Vec2i& operator+=(Vec2i& vector, const Vec2i& rhs);

/// Adds a 3D vector to another 3D vector.
constexpr Vec3& operator+=(Vec3& vector, const Vec3& rhs);

/// Adds a 3D vector to another 3D vector.
constexpr Vec3i& operator+=(Vec3i& vector, const Vec3i& rhs);

/// Adds a 4D vector to another 4D vector.
constexpr Vec4& operator+=(Vec4& vector, const Vec4& rhs);

/// Adds a 4D vector to another 4D vector.
constexpr Vec4i& operator+=(Vec4i& vector, const Vec4i& rhs);

/// Subtracts two 2D vectors.
constexpr Vec2 operator-(const Vec2& lhs, const Vec2& rhs);

/// Negates a 2D vector.
constexpr Vec2 operator-(const Vec2& value);

/// Subtracts two 2D vectors.
constexpr Vec2i operator-(const Vec2i& lhs, const Vec2i& rhs);

/// Negates a 2D vector.
constexpr Vec2i operator-(const Vec2i& value);

/// Subtracts two 3D vectors.
constexpr Vec3 operator-(const Vec3& lhs, const Vec3& rhs);

/// Negates a 3D vector.
constexpr Vec3 operator-(const Vec3& value);

/// Subtracts two 3D vectors.
constexpr Vec3i operator-(const Vec3i& lhs, const Vec3i& rhs);

/// Negates a 3D vector.
constexpr Vec3i operator-(const Vec3i& value);

/// Subtracts two 4D vectors.
constexpr Vec4 operator-(const Vec4& lhs, const Vec4& rhs);

/// Negates a 4D vector.
constexpr Vec4 operator-(const Vec4& value);

/// Subtracts two 4D vectors.
constexpr Vec4i operator-(const Vec4i& lhs, const Vec4i& rhs);

/// Negates a 4D vector.
constexpr Vec4i operator-(const Vec4i& value);

/// Subtracts a 2D vector from another 2D vector.
constexpr Vec2& operator-=(Vec2& lhs, const Vec2& rhs);

/// Subtracts a 2D vector from another 2D vector.
constexpr Vec2i& operator-=(Vec2i& vector, const Vec2i& rhs);

/// Subtracts a 3D vector from another 3D vector.
constexpr Vec3& operator-=(Vec3& vector, const Vec3& rhs);

/// Subtracts a 3D vector from another 3D vector.
constexpr Vec3i& operator-=(Vec3i& vector, const Vec3i& rhs);

/// Subtracts a 4D vector from another 4D vector.
constexpr Vec4& operator-=(Vec4& vector, const Vec4& rhs);

/// Subtracts a 4D vector from another 4D vector.
constexpr Vec4i& operator-=(Vec4i& vector, const Vec4i& rhs);

/// Multiplies two 2D vectors.
constexpr Vec2 operator*(const Vec2& lhs, const Vec2& rhs);

/// Multiplies a 2D vector by a number.
constexpr Vec2 operator*(const Vec2& lhs, float rhs);

/// Multiplies a 2D vector by a number.
constexpr Vec2 operator*(float lhs, const Vec2& rhs);

/// Transforms a 2D vector by a matrix.
constexpr Vec2 operator*(const Vec2& lhs, const Matrix& rhs);

/// Multiplies two 3D vectors.
constexpr Vec3 operator*(const Vec3& lhs, const Vec3& rhs);

/// Multiplies a 3D vector by a number.
constexpr Vec3 operator*(const Vec3& lhs, float rhs);

/// Multiplies a 3D vector by a number.
constexpr Vec3 operator*(float lhs, const Vec3& rhs);

/// Multiplies two 3D vectors.
constexpr Vec3i operator*(const Vec3i& lhs, const Vec3i& rhs);

/// Multiplies a 3D vector by a number.
constexpr Vec3i operator*(const Vec3i& lhs, int rhs);

/// Multiplies a 3D vector by a number.
constexpr Vec3i operator*(int lhs, const Vec3i& rhs);

/// Multiplies two 4D vectors.
constexpr Vec4 operator*(const Vec4& lhs, const Vec4& rhs);

/// Multiplies a 4D vector by a number.
constexpr Vec4 operator*(const Vec4& lhs, float rhs);

/// Multiplies a 4D vector by a number.
constexpr Vec4 operator*(float lhs, const Vec4& rhs);

/// Multiplies two 4D vectors.
constexpr Vec4i operator*(const Vec4i& lhs, const Vec4i& rhs);

/// Multiplies a 4D vector by a number.
constexpr Vec4i operator*(const Vec4i& lhs, int rhs);

/// Multiplies a 4D vector by a number.
constexpr Vec4i operator*(int lhs, const Vec4i& rhs);

constexpr Matrix operator*(const Matrix& lhs, const Matrix& rhs);

/// Multiplies two 2D vectors.
constexpr Vec2i operator*(const Vec2i& lhs, const Vec2i& rhs);

/// Multiplies a 2D vector by a number.
constexpr Vec2i operator*(const Vec2i& lhs, int rhs);

/// Multiplies a 2D vector by a number.
constexpr Vec2i operator*(int lhs, const Vec2i& rhs);

/// Scales a 2D vector by another 2D vector.
constexpr Vec2& operator*=(Vec2& lhs, const Vec2& rhs);

/// Scales a 2D vector by a number.
constexpr Vec2& operator*=(Vec2& lhs, float rhs);

/// Transforms a 2D vector by a matrix.
constexpr Vec2& operator*=(Vec2& lhs, const Matrix& rhs);

/// Scales a 2D vector by another 2D vector.
constexpr Vec2i& operator*=(Vec2i& vector, const Vec2i& rhs);

/// Scales a 2D vector by a number.
constexpr Vec2i& operator*=(Vec2i& vector, int rhs);

/// Scales a 3D vector by another 3D vector.
constexpr Vec3& operator*=(Vec3& vector, const Vec3& rhs);

/// Scales a 3D vector by a number.
constexpr Vec3& operator*=(Vec3& vector, float rhs);

/// Scales a 3D vector by another 3D vector.
constexpr Vec3i& operator*=(Vec3i& vector, const Vec3i& rhs);

/// Scales a 3D vector by a number.
constexpr Vec3i& operator*=(Vec3i& vector, int rhs);

/// Scales a 4D vector by another 4D vector.
constexpr Vec4& operator*=(Vec4& vector, const Vec4& rhs);

/// Scales a 4D vector by a number.
constexpr Vec4& operator*=(Vec4& vector, float rhs);

/// Scales a 4D vector by another 4D vector.
constexpr Vec4i& operator*=(Vec4i& vector, const Vec4i& rhs);

/// Scales a 4D vector by a number.
constexpr Vec4i& operator*=(Vec4i& vector, int rhs);

/// Divides a 2D vector by another 2D vector.
constexpr Vec2 operator/(const Vec2& lhs, const Vec2& rhs);

/// Divides a 2D vector by a number.
constexpr Vec2 operator/(const Vec2& lhs, float rhs);

/// Divides a 2D vector by another 2D vector.
constexpr Vec2i operator/(const Vec2i& lhs, const Vec2i& rhs);

/// Divides a 2D vector by a number.
constexpr Vec2i operator/(const Vec2i& lhs, int rhs);

/// Divides a 3D vector by another 3D vector.
constexpr Vec3 operator/(const Vec3& lhs, const Vec3& rhs);

/// Divides a 3D vector by a number.
constexpr Vec3 operator/(const Vec3& lhs, float rhs);

/// Divides a 3D vector by another 3D vector.
constexpr Vec3i operator/(const Vec3i& lhs, const Vec3i& rhs);

/// Divides a 3D vector by a number.
constexpr Vec3i operator/(const Vec3i& lhs, int rhs);

/// Divides a 4D vector by another 4D vector.
constexpr Vec4 operator/(const Vec4& lhs, const Vec4& rhs);

/// Divides a 4D vector by a number.
constexpr Vec4 operator/(const Vec4& lhs, float rhs);

/// Divides a 4D vector by another 4D vector.
constexpr Vec4i operator/(const Vec4i& lhs, const Vec4i& rhs);

/// Divides a 4D vector by a number.
constexpr Vec4i operator/(const Vec4i& lhs, int rhs);

/// Divides a 2D vector by another 2D vector.
constexpr Vec2& operator/=(Vec2& lhs, const Vec2& rhs);

/// Divides a 2D vector by a number.
constexpr Vec2& operator/=(Vec2& lhs, float rhs);

/// Divides a 4D vector by another 4D vector.
constexpr Vec4& operator/=(Vec4& vector, const Vec4& rhs);

/// Divides a 4D vector by a number.
constexpr Vec4& operator/=(Vec4& vector, float rhs);

/// Divides a 4D vector by another 4D vector.
constexpr Vec4i& operator/=(Vec4i& vector, const Vec4i& rhs);

/// Divides a 4D vector by a number.
constexpr Vec4i& operator/=(Vec4i& vector, int rhs);

/// Divides a 2D vector by another 2D vector.
constexpr Vec2i& operator/=(Vec2i& vector, const Vec2i& rhs);

/// Divides a 2D vector by a number.
constexpr Vec2i& operator/=(Vec2i& vector, int rhs);

/// Divides a 3D vector by another 3D vector.
constexpr Vec3& operator/=(Vec3& vector, const Vec3& rhs);

/// Divides a 3D vector by a number.
constexpr Vec3& operator/=(Vec3& vector, float rhs);

/// Divides a 3D vector by another 3D vector.
constexpr Vec3i& operator/=(Vec3i& vector, const Vec3i& rhs);

/// Divides a 3D vector by a number.
constexpr Vec3i& operator/=(Vec3i& vector, int rhs);
} // namespace Polly

#include "Polly/Details/LinalgOps.inl"