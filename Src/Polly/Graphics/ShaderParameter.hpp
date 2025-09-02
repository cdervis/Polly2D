// Copyright (C) 2025 Cem Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE.

#pragma once

#include "Polly/Any.hpp"
#include "Polly/String.hpp"

namespace Polly
{
class CBufferPacker;

enum class ShaderParameterType : u8
{
    Float,
    Int,
    Bool,
    Vec2,
    Vec3,
    Vec4,
    Matrix,
    FloatArray,
    IntArray,
    BoolArray,
    Vec2Array,
    Vec3Array,
    Vec4Array,
    MatrixArray,
};

class ShaderParameter final
{
    friend CBufferPacker;

  public:
    static constexpr auto arrayElementBaseAlignment = 16u;

    String              name;
    ShaderParameterType type;

    // For scalar parameters, this is the offset in bytes in the cbuffer.
    // For image parameters, this is the binding slot.
    u16 offset = 0;

    Maybe<u16> sizeInBytes;
    Maybe<u16> arraySize;
    Any        defaultValue;
};

static bool operator==(const ShaderParameter& lhs, const ShaderParameter& rhs)
{
    return lhs.name == rhs.name;
}

static bool operator==(const ShaderParameter& lhs, StringView rhs)
{
    return lhs.name == rhs;
}

static bool operator<(StringView name, const ShaderParameter& parameter)
{
    return name < parameter.name;
}

static bool operator<(const ShaderParameter& parameter, StringView name)
{
    return parameter.name < name;
}
} // namespace Polly