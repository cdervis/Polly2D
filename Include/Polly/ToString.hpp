// Copyright (C) 2025 Cem Dervis
// This file is part of Polly, a 2D C++ game framework for minimalists.
// For conditions of distribution and use, see copyright notice in LICENSE, or https://polly2d.org.

#pragma once

#include "Polly/Concepts.hpp"
#include "Polly/Maybe.hpp"
#include "Polly/Prerequisites.hpp"

namespace Polly
{
class Any;
class Image;
class String;
class StringView;
struct Color;
struct Degrees;
struct GamePerformanceStats;
struct Matrix;
struct Radians;
struct Rectangle;
struct Vec2;
struct Vec3;
struct Vec4;
enum class AnyType;
enum class ImageFormat;
enum class Key;
enum class KeyModifier;
enum class MouseButton;
enum class Scancode;

String toString(short value);
String toString(unsigned short value);
String toString(int value);
String toString(unsigned int value);
String toString(long value);
String toString(unsigned long value);
String toString(long long value);
String toString(unsigned long long value);
String toString(float value);
String toString(double value);
String toString(bool value);
String toString(AnyType value);
String toString(const Any& value);
String toString(const String& value);
String toString(const StringView& value);
String toString(const char* value);
String toString(const Vec2& value);
String toString(const Vec3& value);
String toString(const Vec4& value);
String toString(const Color& value);
String toString(const Matrix& value);
String toString(const Rectangle& value);
String toString(Degrees value);
String toString(Radians value);
String toString(MouseButton value);
String toString(Key value);
String toString(KeyModifier value);
String toString(Scancode value);
String toString(ImageFormat value);
String toString(const Image& value);
String toString(const GamePerformanceStats& value);
String toString(const void* value);
String toString(Details::NoObjectTag);
} // namespace Polly

#include "Polly/Details/magic_enum.hpp"
#include "Polly/String.hpp"

namespace Polly
{
namespace Concepts
{
template<typename T>
concept ToStringInvocableThroughPointer = requires(T t) {
    { t->toString() } -> std::convertible_to<String>;
};

template<typename T>
concept HasToStringMember = requires(T t) {
    { t.toString() } -> std::convertible_to<String>;
} or ToStringInvocableThroughPointer<T>;

template<typename T>
concept HasToString = requires(T t) {
    { Polly::toString(t) } -> std::convertible_to<String>;
} or HasToStringMember<T>;
} // namespace Concepts

template<typename T>
static String toString(const T* value)
{
    return toString(static_cast<const void*>(value));
}

template<typename T>
requires(Concepts::ToStringInvocableThroughPointer<std::remove_cvref_t<T>>)
static String toString(const T& value)
{
    return value->toString();
}

template<typename T>
requires(Concepts::HasToStringMember<std::remove_cvref_t<T>>)
static String toString(const T& value)
{
    return value.toString();
}

template<typename T>
requires(std::is_enum_v<T>)
static String toString(T value)
{
    return String(magic_enum::enum_name(value));
}

inline String toString(Details::NoObjectTag)
{
    return "none";
}

template<typename T>
requires(Concepts::HasToString<std::remove_cvref_t<T>>)
static String toString(const Maybe<T>& value)
{
    return value ? toString(*value) : "none"_s;
}

template<Concepts::ForwardContainer T>
requires(Concepts::HasToString<typename T::value_type>)
static String toString(const T& container)
{
    auto str = String();
    str += '[';

    for (const auto& value : container)
    {
        str += toString(value);
        str += "; ";
    }

    if (!container.isEmpty())
    {
        str.removeLast(2);
    }

    str += ']';

    return str;
}
} // namespace Polly