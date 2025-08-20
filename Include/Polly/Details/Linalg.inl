// Copyright (C) 2025 Cemalettin Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE.

#pragma once

namespace Polly
{
constexpr Vec2::Vec2() = default;

constexpr Vec2::Vec2(const float xy)
    : x(xy)
    , y(xy)
{
}

constexpr Vec2::Vec2(const float x, const float y)
    : x(x)
    , y(y)
{
}

constexpr Vec2i::Vec2i() = default;

constexpr Vec2i::Vec2i(const int xy)
    : x(xy)
    , y(xy)
{
}

constexpr Vec2i::Vec2i(const int x, const int y)
    : x(x)
    , y(y)
{
}

constexpr Vec2i::Vec2i(const Vec2& vector)
    : x(static_cast<int>(vector.x))
    , y(static_cast<int>(vector.y))
{
}

constexpr Vec2ui::Vec2ui()
    : x(0u)
    , y(0u)
{
}

constexpr Vec2ui::Vec2ui(const unsigned int xy)
    : x(xy)
    , y(xy)
{
}

constexpr Vec2ui::Vec2ui(const unsigned int x, const unsigned int y)
    : x(x)
    , y(y)
{
}

constexpr Vec3::Vec3() = default;

constexpr Vec3::Vec3(const float xyz)
    : x(xyz)
    , y(xyz)
    , z(xyz)
{
}

constexpr Vec3::Vec3(const float x, const float y, const float z)
    : x(x)
    , y(y)
    , z(z)
{
}

constexpr Vec3i::Vec3i() = default;

constexpr Vec3i::Vec3i(const int xyz)
    : x(xyz)
    , y(xyz)
    , z(xyz)
{
}

constexpr Vec3i::Vec3i(const int x, const int y, const int z)
    : x(x)
    , y(y)
    , z(z)
{
}

constexpr Vec4::Vec4() = default;

constexpr Vec4::Vec4(float xyzw)
    : x(xyzw)
    , y(xyzw)
    , z(xyzw)
    , w(xyzw)
{
}

constexpr Vec4::Vec4(const float x, const float y, const float z, const float w)
    : x(x)
    , y(y)
    , z(z)
    , w(w)
{
}

constexpr Vec4::Vec4(const Vec2 xy, const Vec2 zw)
    : x(xy.x)
    , y(xy.y)
    , z(zw.x)
    , w(zw.y)
{
}

constexpr Vec4::Vec4(const Vec2 xy, const float z, const float w)
    : x(xy.x)
    , y(xy.y)
    , z(z)
    , w(w)
{
}

constexpr Vec4::Vec4(const Vec3 xyz, const float w)
    : x(xyz.x)
    , y(xyz.y)
    , z(xyz.z)
    , w(w)
{
}

constexpr Vec4i::Vec4i() = default;

constexpr Vec4i::Vec4i(const int xyzw)
    : x(xyzw)
    , y(xyzw)
    , z(xyzw)
    , w(xyzw)
{
}

constexpr Vec4i::Vec4i(const int x, const int y, const int z, const int w)
    : x(x)
    , y(y)
    , z(z)
    , w(w)
{
}

constexpr Vec4i::Vec4i(const Vec2i xy, const Vec2i zw)
    : x(xy.x)
    , y(xy.y)
    , z(zw.x)
    , w(zw.y)
{
}

constexpr Vec4i::Vec4i(const Vec2i xy, const int z, const int w)
    : x(xy.x)
    , y(xy.y)
    , z(z)
    , w(w)
{
}

constexpr Vec4i::Vec4i(const Vec3i xyz, const int w)
    : x(xyz.x)
    , y(xyz.y)
    , z(xyz.z)
    , w(w)
{
}

inline const float* Matrix::data() const
{
    return &row1.x;
}

inline float* Matrix::begin()
{
    return &row1.x;
}

inline const float* Matrix::begin() const
{
    return &row1.x;
}

inline const float* Matrix::cbegin() const
{
    return &row1.x;
}

inline float* Matrix::end()
{
    return &row1.x + 16;
}

inline const float* Matrix::end() const
{
    return &row1.x + 16;
}

inline const float* Matrix::cend() const
{
    return &row1.x + 16;
}

constexpr Matrix::Matrix()
    : row1(1, 0, 0, 0)
    , row2(0, 1, 0, 0)
    , row3(0, 0, 1, 0)
    , row4(0, 0, 0, 1)
{
}


constexpr Matrix::Matrix(const Vec4 row1, const Vec4 row2, const Vec4 row3, const Vec4 row4)
    : row1(row1)
    , row2(row2)
    , row3(row3)
    , row4(row4)
{
}

constexpr Matrix::Matrix(float diagonal_value)
    : row1(diagonal_value)
    , row2(diagonal_value)
    , row3(diagonal_value)
    , row4(diagonal_value)
{
}
} // namespace Polly