// Copyright (C) 2025 Cem Dervis
// This file is part of Polly, a 2D C++ game framework for minimalists.
// For conditions of distribution and use, see copyright notice in LICENSE, or https://polly2d.org.

#pragma once

#include "Polly/Linalg.hpp"
#include "Polly/Math.hpp"
#include "Polly/Radians.hpp"

inline float Polly::length(Vec2 vector)
{
    return sqrt(lengthSquared(vector));
}

constexpr float Polly::lengthSquared(Vec2 vector)
{
    return (vector.x * vector.x) + (vector.y * vector.y);
}

inline Polly::Vec2 Polly::normalize(Vec2 vector)
{
    const auto len = length(vector);

    return isZero(len) ? Vec2() : vector / len;
}

inline Polly::Vec2 Polly::round(Vec2 vector)
{
    return Vec2(round(vector.x), round(vector.y));
}

inline Polly::Vec2 Polly::abs(Vec2 vector)
{
    return Vec2(abs(vector.x), abs(vector.y));
}

inline Polly::Vec2 Polly::sin(Vec2 vector)
{
    return Vec2(sin(vector.x), sin(vector.y));
}

inline Polly::Vec2 Polly::cos(Vec2 vector)
{
    return Vec2(cos(vector.x), cos(vector.y));
}

inline Polly::Vec2 Polly::tan(Vec2 vector)
{
    return Vec2(tan(vector.x), tan(vector.y));
}

inline Polly::Vec2 Polly::pow(Vec2 x, Vec2 y)
{
    return Vec2(pow(x.x, y.x), pow(x.y, y.y));
}

inline Polly::Vec2 Polly::floor(Vec2 value)
{
    return Vec2(floor(value.x), floor(value.y));
}

inline Polly::Vec2 Polly::ceil(Vec2 value)
{
    return Vec2(ceil(value.x), ceil(value.y));
}

constexpr float Polly::dot(Vec2 lhs, Vec2 rhs)
{
    return (lhs.x * rhs.x) + (lhs.y * rhs.y);
}

inline float Polly::distance(Vec2 lhs, Vec2 rhs)
{
    return length(rhs - lhs);
}

constexpr float Polly::distanceSquared(Vec2 lhs, Vec2 rhs)
{
    return lengthSquared(rhs - lhs);
}

constexpr Polly::Vec2 Polly::lerp(Vec2 start, Vec2 end, float t)
{
    return Vec2(lerp(start.x, end.x, t), lerp(start.y, end.y, t));
}

constexpr Polly::Vec2 Polly::smoothstep(Vec2 start, Vec2 end, float t)
{
    return Vec2(smoothstep(start.x, end.x, t), smoothstep(start.y, end.y, t));
}

constexpr Polly::Vec2 Polly::clamp(Vec2 value, Vec2 min, Vec2 max)
{
    return Vec2(clamp(value.x, min.x, max.x), clamp(value.y, min.y, max.y));
}

constexpr bool Polly::isZero(Vec2 vector)
{
    return isZero(vector.x) && isZero(vector.y);
}

constexpr bool Polly::areEqualWithin(Vec2 lhs, Vec2 rhs, float threshold)
{
    return areEqualWithin(lhs.x, rhs.x, threshold) && areEqualWithin(lhs.y, rhs.y, threshold);
}

constexpr Polly::Vec2 Polly::min(Vec2 lhs, Vec2 rhs)
{
    return Vec2(min(lhs.x, rhs.x), min(lhs.y, rhs.y));
}

constexpr Polly::Vec2 Polly::max(Vec2 lhs, Vec2 rhs)
{
    return Vec2(max(lhs.x, rhs.x), max(lhs.y, rhs.y));
}

inline Polly::Vec2 Polly::lineNormal(Vec2 start, Vec2 end)
{
    const auto dx = end.x - start.x;
    const auto dy = end.y - start.y;

    return normalize(Vec2(-dy, dx));
}

inline Polly::Vec2 Polly::snapToGrid(Vec2 value, Vec2 grid_size)
{
    return Vec2(snapToGrid(value.x, grid_size.x), snapToGrid(value.y, grid_size.y));
}

constexpr Polly::Vec2& Polly::operator+=(Vec2& lhs, Vec2 rhs)
{
    lhs.x += rhs.x;
    lhs.y += rhs.y;
    return lhs;
}

constexpr Polly::Vec2& Polly::operator-=(Vec2& lhs, Vec2 rhs)
{
    lhs.x -= rhs.x;
    lhs.y -= rhs.y;
    return lhs;
}

constexpr Polly::Vec2& Polly::operator*=(Vec2& lhs, Vec2 rhs)
{
    lhs.x *= rhs.x;
    lhs.y *= rhs.y;
    return lhs;
}

constexpr Polly::Vec2& Polly::operator*=(Vec2& lhs, Matrix rhs)
{
    lhs = lhs * rhs;
    return lhs;
}

constexpr Polly::Vec2& Polly::operator*=(Vec2& lhs, float rhs)
{
    lhs.x *= rhs;
    lhs.y *= rhs;
    return lhs;
}

constexpr Polly::Vec2& Polly::operator/=(Vec2& lhs, Vec2 rhs)
{
    lhs.x /= rhs.x;
    lhs.y /= rhs.y;
    return lhs;
}

constexpr Polly::Vec2& Polly::operator/=(Vec2& lhs, float rhs)
{
    lhs.x /= rhs;
    lhs.y /= rhs;
    return lhs;
}

constexpr Polly::Vec2 Polly::operator-(Vec2 value)
{
    return Vec2(-value.x, -value.y);
}

constexpr Polly::Vec2 Polly::operator+(Vec2 lhs, Vec2 rhs)
{
    return Vec2(lhs.x + rhs.x, lhs.y + rhs.y);
}

constexpr Polly::Vec2 Polly::operator-(Vec2 lhs, Vec2 rhs)
{
    return Vec2(lhs.x - rhs.x, lhs.y - rhs.y);
}

constexpr Polly::Vec2 Polly::operator*(Vec2 lhs, Vec2 rhs)
{
    return Vec2(lhs.x * rhs.x, lhs.y * rhs.y);
}

constexpr Polly::Vec2 Polly::operator*(Vec2 lhs, float rhs)
{
    return Vec2(lhs.x * rhs, lhs.y * rhs);
}

constexpr Polly::Vec2 Polly::operator*(float lhs, Vec2 rhs)
{
    return rhs * lhs;
}

constexpr Polly::Vec2 Polly::operator*(Vec2 lhs, Matrix rhs)
{
    return Vec2(
        (lhs.x * rhs.row1.x) + (lhs.y * rhs.row2.x) + rhs.row4.x,
        (lhs.x * rhs.row1.y) + (lhs.y * rhs.row2.y) + rhs.row4.y);
}

constexpr Polly::Vec2 Polly::operator/(Vec2 lhs, Vec2 rhs)
{
    return Vec2(lhs.x / rhs.x, lhs.y / rhs.y);
}

constexpr Polly::Vec2 Polly::operator/(Vec2 lhs, float rhs)
{
    return Vec2(lhs.x / rhs, lhs.y / rhs);
}

inline Polly::Vec2i Polly::abs(Vec2i vector)
{
    return Vec2i(abs(vector.x), abs(vector.y));
}

constexpr Polly::Vec2i Polly::clamp(Vec2i value, Vec2i min, Vec2i max)
{
    return {
        clamp(value.x, min.x, max.x),
        clamp(value.y, min.y, max.y),
    };
}

constexpr bool Polly::isZero(Vec2i vector)
{
    return vector.x == 0 && vector.y == 0;
}

constexpr Polly::Vec2i Polly::min(Vec2i lhs, Vec2i rhs)
{
    return Vec2i(min(lhs.x, rhs.x), min(lhs.y, rhs.y));
}

constexpr Polly::Vec2i Polly::max(Vec2i lhs, Vec2i rhs)
{
    return Vec2i(max(lhs.x, rhs.x), max(lhs.y, rhs.y));
}

constexpr Polly::Vec2i& Polly::operator+=(Vec2i& vector, Vec2i rhs)
{
    vector.x += rhs.x;
    vector.y += rhs.y;
    return vector;
}

constexpr Polly::Vec2i& Polly::operator-=(Vec2i& vector, Vec2i rhs)
{
    vector.x -= rhs.x;
    vector.y -= rhs.y;
    return vector;
}

constexpr Polly::Vec2i& Polly::operator*=(Vec2i& vector, Vec2i rhs)
{
    vector.x *= rhs.x;
    vector.y *= rhs.y;
    return vector;
}

constexpr Polly::Vec2i& Polly::operator*=(Vec2i& vector, int rhs)
{
    vector.x *= rhs;
    vector.y *= rhs;
    return vector;
}

constexpr Polly::Vec2i& Polly::operator/=(Vec2i& vector, Vec2i rhs)
{
    vector.x /= rhs.x;
    vector.y /= rhs.y;
    return vector;
}

constexpr Polly::Vec2i& Polly::operator/=(Vec2i& vector, int rhs)
{
    vector.x /= rhs;
    vector.y /= rhs;
    return vector;
}

constexpr Polly::Vec2i Polly::operator-(Vec2i value)
{
    return Vec2i(-value.x, -value.y);
}

constexpr Polly::Vec2i Polly::operator+(Vec2i lhs, Vec2i rhs)
{
    return Vec2i(lhs.x + rhs.x, lhs.y + rhs.y);
}

constexpr Polly::Vec2i Polly::operator-(Vec2i lhs, Vec2i rhs)
{
    return {
        lhs.x - rhs.x,
        lhs.y - rhs.y,
    };
}

constexpr Polly::Vec2i Polly::operator*(Vec2i lhs, Vec2i rhs)
{
    return Vec2i(lhs.x * rhs.x, lhs.y * rhs.y);
}

constexpr Polly::Vec2i Polly::operator*(Vec2i lhs, int rhs)
{
    return Vec2i(lhs.x * rhs, lhs.y * rhs);
}

constexpr Polly::Vec2i Polly::operator*(int lhs, Vec2i rhs)
{
    return rhs * lhs;
}

constexpr Polly::Vec2i Polly::operator/(Vec2i lhs, Vec2i rhs)
{
    return Vec2i(lhs.x / rhs.x, lhs.y / rhs.y);
}

constexpr Polly::Vec2i Polly::operator/(Vec2i lhs, int rhs)
{
    return Vec2i(lhs.x / rhs, lhs.y / rhs);
}

inline float Polly::length(Vec3 vector)
{
    return sqrt(lengthSquared(vector));
}

constexpr float Polly::lengthSquared(Vec3 vector)
{
    return (vector.x * vector.x) + (vector.y * vector.y) + (vector.z * vector.z);
}

inline Polly::Vec3 Polly::normalize(Vec3 vector)
{
    const auto len = length(vector);
    return isZero(len) ? Vec3() : vector / len;
}

inline Polly::Vec3 Polly::round(Vec3 vector)
{
    return Vec3(round(vector.x), round(vector.y), round(vector.z));
}

inline Polly::Vec3 Polly::abs(Vec3 vector)
{
    return Vec3(abs(vector.x), abs(vector.y), abs(vector.z));
}

inline Polly::Vec3 Polly::sin(Vec3 vector)
{
    return Vec3(sin(vector.x), sin(vector.y), sin(vector.z));
}

inline Polly::Vec3 Polly::cos(Vec3 vector)
{
    return Vec3(cos(vector.x), cos(vector.y), cos(vector.z));
}

inline Polly::Vec3 Polly::tan(Vec3 vector)
{
    return Vec3(tan(vector.x), tan(vector.y), tan(vector.z));
}

inline Polly::Vec3 Polly::pow(Vec3 base, Vec3 exp)
{
    return Vec3(pow(base.x, exp.x), pow(base.y, exp.y), pow(base.z, exp.z));
}

inline Polly::Vec3 Polly::floor(Vec3 value)
{
    return Vec3(floor(value.x), floor(value.y), floor(value.z));
}

inline Polly::Vec3 Polly::ceil(Vec3 value)
{
    return Vec3(ceil(value.x), ceil(value.y), ceil(value.z));
}

inline float Polly::dot(Vec3 lhs, Vec3 rhs)
{
    return (lhs.x * rhs.x) + (lhs.y * rhs.y) + (lhs.z * rhs.z);
}

inline Polly::Vec3 Polly::cross(Vec3 lhs, Vec3 rhs)
{
    return Vec3(
        (lhs.y * rhs.z) - (rhs.y * lhs.z),
        (lhs.z * rhs.x) - (rhs.z * lhs.x),
        (lhs.x * rhs.y) - (rhs.x * lhs.y));
}

inline float Polly::distance(Vec3 lhs, Vec3 rhs)
{
    return length(rhs - lhs);
}

inline float Polly::distanceSquared(Vec3 lhs, Vec3 rhs)
{
    return lengthSquared(rhs - lhs);
}

constexpr Polly::Vec3 Polly::lerp(Vec3 start, Vec3 end, float t)
{
    return Vec3(lerp(start.x, end.x, t), lerp(start.y, end.y, t), lerp(start.z, end.z, t));
}

constexpr Polly::Vec3 Polly::smoothstep(Vec3 start, Vec3 end, float t)
{
    return Vec3(smoothstep(start.x, end.x, t), smoothstep(start.y, end.y, t), smoothstep(start.z, end.z, t));
}

constexpr Polly::Vec3 Polly::clamp(Vec3 value, Vec3 min, Vec3 max)
{
    return Vec3(clamp(value.x, min.x, max.x), clamp(value.y, min.y, max.y), clamp(value.z, min.z, max.z));
}

constexpr bool Polly::isZero(Vec3 vector)
{
    return isZero(vector.x) && isZero(vector.y) && isZero(vector.z);
}

constexpr bool Polly::areEqualWithin(Vec3 lhs, Vec3 rhs, float threshold)
{
    return areEqualWithin(lhs.x, rhs.x, threshold)
           && areEqualWithin(lhs.y, rhs.y, threshold)
           && areEqualWithin(lhs.z, rhs.z, threshold);
}

constexpr Polly::Vec3 Polly::min(Vec3 lhs, Vec3 rhs)
{
    return Vec3(min(lhs.x, rhs.x), min(lhs.y, rhs.y), min(lhs.z, rhs.z));
}

constexpr Polly::Vec3 Polly::max(Vec3 lhs, Vec3 rhs)
{
    return Vec3(max(lhs.x, rhs.x), max(lhs.y, rhs.y), max(lhs.z, rhs.z));
}

constexpr Polly::Vec3& Polly::operator+=(Vec3& vector, Vec3 rhs)
{
    vector.x += rhs.x;
    vector.y += rhs.y;
    vector.z += rhs.z;
    return vector;
}

constexpr Polly::Vec3& Polly::operator-=(Vec3& vector, Vec3 rhs)
{
    vector.x -= rhs.x;
    vector.y -= rhs.y;
    vector.z -= rhs.z;
    return vector;
}

constexpr Polly::Vec3& Polly::operator*=(Vec3& vector, Vec3 rhs)
{
    vector.x *= rhs.x;
    vector.y *= rhs.y;
    vector.z *= rhs.z;
    return vector;
}

constexpr Polly::Vec3& Polly::operator*=(Vec3& vector, float rhs)
{
    vector.x *= rhs;
    vector.y *= rhs;
    vector.z *= rhs;
    return vector;
}

constexpr Polly::Vec3& Polly::operator/=(Vec3& vector, Vec3 rhs)
{
    vector.x /= rhs.x;
    vector.y /= rhs.y;
    vector.z /= rhs.z;
    return vector;
}

constexpr Polly::Vec3& Polly::operator/=(Vec3& vector, float rhs)
{
    vector.x /= rhs;
    vector.y /= rhs;
    vector.z /= rhs;
    return vector;
}

constexpr Polly::Vec3 Polly::operator-(Vec3 value)
{
    return Vec3(-value.x, -value.y, -value.z);
}

constexpr Polly::Vec3 Polly::operator+(Vec3 lhs, Vec3 rhs)
{
    return Vec3(lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z);
}

constexpr Polly::Vec3 Polly::operator-(Vec3 lhs, Vec3 rhs)
{
    return Vec3(lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z);
}

constexpr Polly::Vec3 Polly::operator*(Vec3 lhs, Vec3 rhs)
{
    return Vec3(lhs.x * rhs.x, lhs.y * rhs.y, lhs.z * rhs.z);
}

constexpr Polly::Vec3 Polly::operator*(Vec3 lhs, float rhs)
{
    return Vec3(lhs.x * rhs, lhs.y * rhs, lhs.z * rhs);
}

constexpr Polly::Vec3 Polly::operator*(float lhs, Vec3 rhs)
{
    return rhs * lhs;
}

constexpr Polly::Vec3 Polly::operator/(Vec3 lhs, Vec3 rhs)
{
    return Vec3(lhs.x / rhs.x, lhs.y / rhs.y, lhs.z / rhs.z);
}

constexpr Polly::Vec3 Polly::operator/(Vec3 lhs, float rhs)
{
    return Vec3(lhs.x / rhs, lhs.y / rhs, lhs.z / rhs);
}

inline Polly::Vec3i Polly::abs(Vec3i vector)
{
    return Vec3i(abs(vector.x), abs(vector.y), abs(vector.z));
}

constexpr Polly::Vec3i Polly::clamp(Vec3i value, Vec3i min, Vec3i max)
{
    return Vec3i(clamp(value.x, min.x, max.x), clamp(value.y, min.y, max.y), clamp(value.z, min.z, max.z));
}

constexpr bool Polly::isZero(Vec3i vector)
{
    return vector.x == 0 && vector.y == 0 && vector.z == 0;
}

constexpr Polly::Vec3i Polly::min(Vec3i lhs, Vec3i rhs)
{
    return Vec3i(min(lhs.x, rhs.x), min(lhs.y, rhs.y), min(lhs.z, rhs.z));
}

constexpr Polly::Vec3i Polly::max(Vec3i lhs, Vec3i rhs)
{
    return Vec3i(max(lhs.x, rhs.x), max(lhs.y, rhs.y), max(lhs.z, rhs.z));
}

constexpr Polly::Vec3i& Polly::operator+=(Vec3i& vector, Vec3i rhs)
{
    vector.x += rhs.x;
    vector.y += rhs.y;
    vector.z += rhs.z;
    return vector;
}

constexpr Polly::Vec3i& Polly::operator-=(Vec3i& vector, Vec3i rhs)
{
    vector.x -= rhs.x;
    vector.y -= rhs.y;
    vector.z -= rhs.z;
    return vector;
}

constexpr Polly::Vec3i& Polly::operator*=(Vec3i& vector, Vec3i rhs)
{
    vector.x *= rhs.x;
    vector.y *= rhs.y;
    vector.z *= rhs.z;
    return vector;
}

constexpr Polly::Vec3i& Polly::operator*=(Vec3i& vector, int rhs)
{
    vector.x *= rhs;
    vector.y *= rhs;
    vector.z *= rhs;
    return vector;
}

constexpr Polly::Vec3i& Polly::operator/=(Vec3i& vector, Vec3i rhs)
{
    vector.x /= rhs.x;
    vector.y /= rhs.y;
    vector.z /= rhs.z;
    return vector;
}

constexpr Polly::Vec3i& Polly::operator/=(Vec3i& vector, int rhs)
{
    vector.x /= rhs;
    vector.y /= rhs;
    vector.z /= rhs;
    return vector;
}

constexpr Polly::Vec3i Polly::operator-(Vec3i value)
{
    return Vec3i(-value.x, -value.y, -value.z);
}

constexpr Polly::Vec3i Polly::operator+(Vec3i lhs, Vec3i rhs)
{
    return Vec3i(lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z);
}

constexpr Polly::Vec3i Polly::operator-(Vec3i lhs, Vec3i rhs)
{
    return Vec3i(lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z);
}

constexpr Polly::Vec3i Polly::operator*(Vec3i lhs, Vec3i rhs)
{
    return Vec3i(lhs.x * rhs.x, lhs.y * rhs.y, lhs.z * rhs.z);
}

constexpr Polly::Vec3i Polly::operator*(Vec3i lhs, int rhs)
{
    return Vec3i(lhs.x * rhs, lhs.y * rhs, lhs.z * rhs);
}

constexpr Polly::Vec3i Polly::operator*(int lhs, Vec3i rhs)
{
    return rhs * lhs;
}

constexpr Polly::Vec3i Polly::operator/(Vec3i lhs, Vec3i rhs)
{
    return Vec3i(lhs.x / rhs.x, lhs.y / rhs.y, lhs.z / rhs.z);
}

constexpr Polly::Vec3i Polly::operator/(Vec3i lhs, int rhs)
{
    return Vec3i(lhs.x / rhs, lhs.y / rhs, lhs.z / rhs);
}

constexpr Polly::Matrix Polly::transpose(Matrix matrix)
{
    return Matrix(
        Vec4(matrix.row1.x, matrix.row2.x, matrix.row3.x, matrix.row4.x),
        Vec4(matrix.row1.y, matrix.row2.y, matrix.row3.y, matrix.row4.y),
        Vec4(matrix.row1.z, matrix.row2.z, matrix.row3.z, matrix.row4.z),
        Vec4(matrix.row1.w, matrix.row2.w, matrix.row3.w, matrix.row4.w));
}

constexpr Polly::Matrix Polly::translate(const Vec2 translation)
{
    const auto x = translation.x;
    const auto y = translation.y;

    return Matrix(Vec4(1, 0, 0, 0), Vec4(0, 1, 0, 0), Vec4(0, 0, 1, 0), Vec4(x, y, 0, 1));
}

constexpr Polly::Matrix Polly::scale(const Vec2 scale)
{
    return Matrix(Vec4(scale.x, 0, 0, 0), Vec4(0, scale.y, 0, 0), Vec4(0, 0, 1, 0), Vec4(0, 0, 0, 1));
}

inline Polly::Matrix Polly::rotate(const Radians radians)
{
    const auto c = cos(radians.value);
    const auto s = sin(radians.value);

    return Matrix(Vec4(c, s, 0, 0), Vec4(-s, c, 0, 0), Vec4(0, 0, 1, 0), Vec4(0, 0, 0, 1));
}

constexpr bool Polly::areEqualWithin(Matrix lhs, Matrix rhs, float threshold)
{
    return areEqualWithin(lhs.row1.x, rhs.row1.x, threshold)
           && areEqualWithin(lhs.row1.y, rhs.row1.y, threshold)
           && areEqualWithin(lhs.row1.z, rhs.row1.z, threshold)
           && areEqualWithin(lhs.row1.w, rhs.row1.w, threshold)
           && areEqualWithin(lhs.row2.x, rhs.row2.x, threshold)
           && areEqualWithin(lhs.row2.y, rhs.row2.y, threshold)
           && areEqualWithin(lhs.row2.z, rhs.row2.z, threshold)
           && areEqualWithin(lhs.row2.w, rhs.row2.w, threshold)
           && areEqualWithin(lhs.row3.x, rhs.row3.x, threshold)
           && areEqualWithin(lhs.row3.y, rhs.row3.y, threshold)
           && areEqualWithin(lhs.row3.z, rhs.row3.z, threshold)
           && areEqualWithin(lhs.row3.w, rhs.row3.w, threshold)
           && areEqualWithin(lhs.row4.x, rhs.row4.x, threshold)
           && areEqualWithin(lhs.row4.y, rhs.row4.y, threshold)
           && areEqualWithin(lhs.row4.z, rhs.row4.z, threshold)
           && areEqualWithin(lhs.row4.w, rhs.row4.w, threshold);
}

constexpr Polly::Matrix Polly::operator*(Matrix lhs, Matrix rhs)
{
    const auto l11 = lhs.row1.x;
    const auto l12 = lhs.row1.y;
    const auto l13 = lhs.row1.z;
    const auto l14 = lhs.row1.w;

    const auto l21 = lhs.row2.x;
    const auto l22 = lhs.row2.y;
    const auto l23 = lhs.row2.z;
    const auto l24 = lhs.row2.w;

    const auto l31 = lhs.row3.x;
    const auto l32 = lhs.row3.y;
    const auto l33 = lhs.row3.z;
    const auto l34 = lhs.row3.w;

    const auto l41 = lhs.row4.x;
    const auto l42 = lhs.row4.y;
    const auto l43 = lhs.row4.z;
    const auto l44 = lhs.row4.w;

    const auto r11 = rhs.row1.x;
    const auto r12 = rhs.row1.y;
    const auto r13 = rhs.row1.z;
    const auto r14 = rhs.row1.w;

    const auto r21 = rhs.row2.x;
    const auto r22 = rhs.row2.y;
    const auto r23 = rhs.row2.z;
    const auto r24 = rhs.row2.w;

    const auto r31 = rhs.row3.x;
    const auto r32 = rhs.row3.y;
    const auto r33 = rhs.row3.z;
    const auto r34 = rhs.row3.w;

    const auto r41 = rhs.row4.x;
    const auto r42 = rhs.row4.y;
    const auto r43 = rhs.row4.z;
    const auto r44 = rhs.row4.w;

    return Matrix(
        Vec4(
            (l11 * r11) + (l12 * r21) + (l13 * r31) + (l14 * r41),
            (l11 * r12) + (l12 * r22) + (l13 * r32) + (l14 * r42),
            (l11 * r13) + (l12 * r23) + (l13 * r33) + (l14 * r43),
            (l11 * r14) + (l12 * r24) + (l13 * r34) + (l14 * r44)),
        Vec4(
            (l21 * r11) + (l22 * r21) + (l23 * r31) + (l24 * r41),
            (l21 * r12) + (l22 * r22) + (l23 * r32) + (l24 * r42),
            (l21 * r13) + (l22 * r23) + (l23 * r33) + (l24 * r43),
            (l21 * r14) + (l22 * r24) + (l23 * r34) + (l24 * r44)),
        Vec4(
            (l31 * r11) + (l32 * r21) + (l33 * r31) + (l34 * r41),
            (l31 * r12) + (l32 * r22) + (l33 * r32) + (l34 * r42),
            (l31 * r13) + (l32 * r23) + (l33 * r33) + (l34 * r43),
            (l31 * r14) + (l32 * r24) + (l33 * r34) + (l34 * r44)),
        Vec4(
            (l41 * r11) + (l42 * r21) + (l43 * r31) + (l44 * r41),
            (l41 * r12) + (l42 * r22) + (l43 * r32) + (l44 * r42),
            (l41 * r13) + (l42 * r23) + (l43 * r33) + (l44 * r43),
            (l41 * r14) + (l42 * r24) + (l43 * r34) + (l44 * r44)));
}

inline float Polly::length(Vec4 vector)
{
    return sqrt(lengthSquared(vector));
}

constexpr float Polly::lengthSquared(Vec4 vector)
{
    return (vector.x * vector.x) + (vector.y * vector.y) + (vector.z * vector.z) + (vector.w * vector.w);
}

inline Polly::Vec4 Polly::normalize(Vec4 vector)
{
    const float len = length(vector);
    return isZero(len) ? Vec4() : vector / len;
}

inline Polly::Vec4 Polly::round(Vec4 vector)
{
    return Vec4(round(vector.x), round(vector.y), round(vector.z), round(vector.w));
}

inline Polly::Vec4 Polly::abs(Vec4 vector)
{
    return Vec4(abs(vector.x), abs(vector.y), abs(vector.z), abs(vector.w));
}

inline Polly::Vec4 Polly::sin(Vec4 vector)
{
    return Vec4(sin(vector.x), sin(vector.y), sin(vector.z), sin(vector.w));
}

inline Polly::Vec4 Polly::cos(Vec4 vector)
{
    return Vec4(cos(vector.x), cos(vector.y), cos(vector.z), cos(vector.w));
}

inline Polly::Vec4 Polly::tan(Vec4 vector)
{
    return Vec4(tan(vector.x), tan(vector.y), tan(vector.z), tan(vector.w));
}

inline Polly::Vec4 Polly::pow(Vec4 x, Vec4 y)
{
    return Vec4(pow(x.x, y.x), pow(x.y, y.y), pow(x.z, y.z), pow(x.w, y.w));
}

inline Polly::Vec4 Polly::floor(Vec4 value)
{
    return Vec4(floor(value.x), floor(value.y), floor(value.z), floor(value.w));
}

inline Polly::Vec4 Polly::ceil(Vec4 value)
{
    return Vec4(ceil(value.x), ceil(value.y), ceil(value.z), ceil(value.w));
}

inline float Polly::dot(Vec4 lhs, Vec4 rhs)
{
    return (lhs.x * rhs.x) + (lhs.y * rhs.y) + (lhs.z * rhs.z) + (lhs.w * rhs.w);
}

inline float Polly::distance(Vec4 lhs, Vec4 rhs)
{
    return length(rhs - lhs);
}

inline float Polly::distanceSquared(Vec4 lhs, Vec4 rhs)
{
    return lengthSquared(rhs - lhs);
}

constexpr Polly::Vec4 Polly::lerp(Vec4 start, Vec4 end, float t)
{
    return Vec4(
        lerp(start.x, end.x, t),
        lerp(start.y, end.y, t),
        lerp(start.z, end.z, t),
        lerp(start.w, end.w, t));
}

constexpr Polly::Vec4 Polly::smoothstep(Vec4 start, Vec4 end, float t)
{
    return Vec4(
        smoothstep(start.x, end.x, t),
        smoothstep(start.y, end.y, t),
        smoothstep(start.z, end.z, t),
        smoothstep(start.w, end.w, t));
}

constexpr Polly::Vec4 Polly::clamp(Vec4 value, Vec4 min, Vec4 max)
{
    return Vec4(
        clamp(value.x, min.x, max.x),
        clamp(value.y, min.y, max.y),
        clamp(value.z, min.z, max.z),
        clamp(value.w, min.w, max.w));
}

constexpr bool Polly::isZero(Vec4 vector)
{
    return isZero(vector.x) && isZero(vector.y) && isZero(vector.z) && isZero(vector.w);
}

constexpr bool Polly::areEqualWithin(Vec4 lhs, Vec4 rhs, float threshold)
{
    return areEqualWithin(lhs.x, rhs.x, threshold)
           && areEqualWithin(lhs.y, rhs.y, threshold)
           && areEqualWithin(lhs.z, rhs.z, threshold)
           && areEqualWithin(lhs.w, rhs.w, threshold);
}

constexpr Polly::Vec4 Polly::min(Vec4 lhs, Vec4 rhs)
{
    return Vec4(min(lhs.x, rhs.x), min(lhs.y, rhs.y), min(lhs.z, rhs.z), min(lhs.w, rhs.w));
}

constexpr Polly::Vec4 Polly::max(Vec4 lhs, Vec4 rhs)
{
    return Vec4(max(lhs.x, rhs.x), max(lhs.y, rhs.y), max(lhs.z, rhs.z), max(lhs.w, rhs.w));
}

constexpr Polly::Vec4& Polly::operator+=(Vec4& vector, Vec4 rhs)
{
    vector.x += rhs.x;
    vector.y += rhs.y;
    vector.z += rhs.z;
    vector.w += rhs.w;
    return vector;
}

constexpr Polly::Vec4& Polly::operator-=(Vec4& vector, Vec4 rhs)
{
    vector.x -= rhs.x;
    vector.y -= rhs.y;
    vector.z -= rhs.z;
    vector.w -= rhs.w;
    return vector;
}

constexpr Polly::Vec4& Polly::operator*=(Vec4& vector, Vec4 rhs)
{
    vector.x *= rhs.x;
    vector.y *= rhs.y;
    vector.z *= rhs.z;
    vector.w *= rhs.w;
    return vector;
}

constexpr Polly::Vec4& Polly::operator*=(Vec4& vector, float rhs)
{
    vector.x *= rhs;
    vector.y *= rhs;
    vector.z *= rhs;
    vector.w *= rhs;
    return vector;
}

constexpr Polly::Vec4& Polly::operator/=(Vec4& vector, Vec4 rhs)
{
    vector.x /= rhs.x;
    vector.y /= rhs.y;
    vector.z /= rhs.z;
    vector.w /= rhs.w;
    return vector;
}

constexpr Polly::Vec4& Polly::operator/=(Vec4& vector, float rhs)
{
    vector.x /= rhs;
    vector.y /= rhs;
    vector.z /= rhs;
    vector.w /= rhs;
    return vector;
}

constexpr Polly::Vec4 Polly::operator-(Vec4 value)
{
    return Vec4(-value.x, -value.y, -value.z, -value.w);
}

constexpr Polly::Vec4 Polly::operator+(Vec4 lhs, Vec4 rhs)
{
    return Vec4(lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z, lhs.w + rhs.w);
}

constexpr Polly::Vec4 Polly::operator-(Vec4 lhs, Vec4 rhs)
{
    return Vec4(lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z, lhs.w - rhs.w);
}

constexpr Polly::Vec4 Polly::operator*(Vec4 lhs, Vec4 rhs)
{
    return Vec4(lhs.x * rhs.x, lhs.y * rhs.y, lhs.z * rhs.z, lhs.w * rhs.w);
}

constexpr Polly::Vec4 Polly::operator*(Vec4 lhs, float rhs)
{
    return Vec4(lhs.x * rhs, lhs.y * rhs, lhs.z * rhs, lhs.w * rhs);
}

constexpr Polly::Vec4 Polly::operator*(float lhs, Vec4 rhs)
{
    return rhs * lhs;
}

constexpr Polly::Vec4 Polly::operator/(Vec4 lhs, Vec4 rhs)
{
    return Vec4(lhs.x / rhs.x, lhs.y / rhs.y, lhs.z / rhs.z, lhs.w / rhs.w);
}

constexpr Polly::Vec4 Polly::operator/(Vec4 lhs, float rhs)
{
    return Vec4(lhs.x / rhs, lhs.y / rhs, lhs.z / rhs, lhs.w / rhs);
}

inline Polly::Vec4i Polly::abs(Vec4i vector)
{
    return Vec4i(abs(vector.x), abs(vector.y), abs(vector.z), abs(vector.w));
}

constexpr Polly::Vec4i Polly::clamp(Vec4i value, Vec4i min, Vec4i max)
{
    return Vec4i(
        clamp(value.x, min.x, max.x),
        clamp(value.y, min.y, max.y),
        clamp(value.z, min.z, max.z),
        clamp(value.w, min.w, max.w));
}

constexpr bool Polly::isZero(Vec4i vector)
{
    return vector.x == 0 && vector.y == 0 && vector.z == 0 && vector.w == 0;
}

constexpr Polly::Vec4i Polly::min(Vec4i lhs, Vec4i rhs)
{
    return Vec4i(min(lhs.x, rhs.x), min(lhs.y, rhs.y), min(lhs.z, rhs.z), min(lhs.w, rhs.w));
}

constexpr Polly::Vec4i Polly::max(Vec4i lhs, Vec4i rhs)
{
    return Vec4i(max(lhs.x, rhs.x), max(lhs.y, rhs.y), max(lhs.z, rhs.z), max(lhs.w, rhs.w));
}

constexpr Polly::Vec4i& Polly::operator+=(Vec4i& vector, Vec4i rhs)
{
    vector.x += rhs.x;
    vector.y += rhs.y;
    vector.z += rhs.z;
    vector.w += rhs.w;
    return vector;
}

constexpr Polly::Vec4i& Polly::operator-=(Vec4i& vector, Vec4i rhs)
{
    vector.x -= rhs.x;
    vector.y -= rhs.y;
    vector.z -= rhs.z;
    vector.w -= rhs.w;
    return vector;
}

constexpr Polly::Vec4i& Polly::operator*=(Vec4i& vector, Vec4i rhs)
{
    vector.x *= rhs.x;
    vector.y *= rhs.y;
    vector.z *= rhs.z;
    vector.w *= rhs.w;
    return vector;
}

constexpr Polly::Vec4i& Polly::operator*=(Vec4i& vector, int rhs)
{
    vector.x *= rhs;
    vector.y *= rhs;
    vector.z *= rhs;
    vector.w *= rhs;
    return vector;
}

constexpr Polly::Vec4i& Polly::operator/=(Vec4i& vector, Vec4i rhs)
{
    vector.x /= rhs.x;
    vector.y /= rhs.y;
    vector.z /= rhs.z;
    vector.w /= rhs.w;
    return vector;
}

constexpr Polly::Vec4i& Polly::operator/=(Vec4i& vector, int rhs)
{
    vector.x /= rhs;
    vector.y /= rhs;
    vector.z /= rhs;
    vector.w /= rhs;
    return vector;
}

constexpr Polly::Vec4i Polly::operator-(Vec4i value)
{
    return Vec4i(-value.x, -value.y, -value.z, -value.w);
}

constexpr Polly::Vec4i Polly::operator+(Vec4i lhs, Vec4i rhs)
{
    return Vec4i(lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z, lhs.w + rhs.w);
}

constexpr Polly::Vec4i Polly::operator-(Vec4i lhs, Vec4i rhs)
{
    return Vec4i(lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z, lhs.w - rhs.w);
}

constexpr Polly::Vec4i Polly::operator*(Vec4i lhs, Vec4i rhs)
{
    return Vec4i(lhs.x * rhs.x, lhs.y * rhs.y, lhs.z * rhs.z, lhs.w * rhs.w);
}

constexpr Polly::Vec4i Polly::operator*(Vec4i lhs, int rhs)
{
    return Vec4i(lhs.x * rhs, lhs.y * rhs, lhs.z * rhs, lhs.w * rhs);
}

constexpr Polly::Vec4i Polly::operator*(int lhs, Vec4i rhs)
{
    return rhs * lhs;
}

constexpr Polly::Vec4i Polly::operator/(Vec4i lhs, Vec4i rhs)
{
    return Vec4i(lhs.x / rhs.x, lhs.y / rhs.y, lhs.z / rhs.z, lhs.w / rhs.w);
}

constexpr Polly::Vec4i Polly::operator/(Vec4i lhs, int rhs)
{
    return Vec4i(lhs.x / rhs, lhs.y / rhs, lhs.z / rhs, lhs.w / rhs);
}