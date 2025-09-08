// Copyright (C) 2023-2024 Cem Dervis
// This file is part of Polly, a minimalistic 2D C++ game framework.
// For conditions of distribution and use, see copyright notice in LICENSE, or https://polly2d.org.

#pragma once

#include "Polly/Prerequisites.hpp"
#include "Polly/Span.hpp"
#include "Polly/StringView.hpp"

namespace Polly
{
class String;
class StringView;
class ByteBlob;
enum class AnyType;

template<typename T>
class Maybe;

class String
{
    friend ByteBlob;

  public:
    using value_type                            = char;
    static constexpr auto isContiguousContainer = true;
    static constexpr auto isForwardContainer    = true;
    static constexpr auto smallBufferSize       = 32u; // Including null-terminator.

    constexpr String();

    constexpr String(Details::NoObjectTag);

    String(const char* str);

    String(std::nullptr_t) = PollyDeleteWithReason("Constructing a String from a nullptr is invalid.");

    explicit String(const char* str, u32 size);

    explicit String(StringView view);

    String(const String& other);

    String& operator=(const String& other);

    String& operator=(StringView other);

    String& operator=(const char* other);

    String(String&& other) noexcept;

    String& operator=(String&& other) noexcept;

    ~String() noexcept;

    [[nodiscard]]
    static String repeat(char ch, u32 count);

    [[nodiscard]]
    static String repeat(StringView str, u32 count, StringView separator = StringView());

    static char* rawConcat(char* dst, const char* src);

    static constexpr u32 rawLength(const char* str);

    void assign(const char* str, Maybe<u32> size);

    void clear();

    void reserve(u32 newCapacity);

    void ensureSize(u32 newSize);

    String substring(u32 offset) const;

    String substring(u32 offset, Maybe<u32> size) const;

    Maybe<u32> find(StringView str, u32 offset = 0) const;

    Maybe<u32> find(char ch, u32 offset = 0) const;

    Maybe<u32> findAnyOf(StringView chars, u32 offset = 0) const;

    Maybe<u32> reverseFind(StringView str) const;

    Maybe<u32> reverseFind(StringView str, Maybe<u32> offset) const;

    Maybe<u32> reverseFind(char ch) const;

    Maybe<u32> reverseFind(char ch, Maybe<u32> offset) const;

    bool startsWith(StringView str) const;

    bool endsWith(StringView str) const;

    void trim(SmallList<char> chars);

    void trimStart(SmallList<char> chars);

    void trimEnd(SmallList<char> chars);

    [[nodiscard]]
    String trimmed(SmallList<char> chars) const;

    void transformToLowerCase();

    [[nodiscard]]
    String lowerCased() const;

    void transformToUpperCase();

    [[nodiscard]]
    String upperCased() const;

    void replaceCharacter(char oldChar, char newChar);

    char* data();

    const char* data() const;

    const char* cstring() const;

    char* begin();

    const char* begin() const;

    char* end();

    const char* end() const;

    u32 size() const;

    bool isEmpty() const;

    u32 capacity() const;

    char& first();

    const char& first() const;

    char& last();

    const char& last() const;

    void remove(u32 index);

    void remove(u32 index, Maybe<u32> size);

    void removeFirst(u32 count = 1);

    void removeLast(u32 count = 1);

    [[nodiscard]]
    bool contains(StringView str) const;

    [[nodiscard]]
    bool contains(char ch) const;

    [[nodiscard]]
    u32 count(char ch) const;

    Maybe<int> toInt() const;

    Maybe<unsigned int> toUInt() const;

    Maybe<float> toFloat() const;

    Maybe<double> toDouble() const;

    void insertAt(u32 index, StringView str);

    bool isSmall() const;

    size_t hashCode() const;

    char& operator[](u32 index);

    const char& operator[](u32 index) const;

    void operator+=(const String& other);

    void operator+=(StringView other);

    void operator+=(const char* other);

    void operator+=(char ch);

    operator StringView() const PollyLifetimeBound;

    constexpr operator Span<char>() const PollyLifetimeBound
    {
        return Span(_data, _size);
    }

    constexpr operator MutableSpan<char>() PollyLifetimeBound
    {
        return MutableSpan(_data, _size);
    }

  private:
    void destroy();

    void append(StringView str);

    void moveFrom(String&& other);

    void checkAccess() const;

    void checkAccessAt(u32 idx) const;

    char  _smallBuffer[smallBufferSize] = {};
    u32   _size                         = 0;
    u32   _capacity                     = smallBufferSize - 1;
    char* _data                         = nullptr;
};

bool operator<(const String& lhs, const String& rhs);

bool operator<(const String& lhs, const StringView& rhs);

bool operator<=(const String& lhs, const String& rhs);

bool operator<=(const String& lhs, const StringView& rhs);

bool operator>(const String& lhs, const String& rhs);

bool operator>(const String& lhs, const StringView& rhs);

bool operator>=(const String& lhs, const String& rhs);

bool operator>=(const String& lhs, const StringView& rhs);

bool operator==(const String& lhs, const String& rhs);

bool operator==(const String& lhs, StringView rhs);

bool operator==(StringView lhs, const String& rhs);

bool operator==(const String& lhs, const char* rhs);

bool operator!=(const String& lhs, const String& rhs);

bool operator!=(const String& lhs, StringView rhs);

bool operator!=(StringView lhs, const String& rhs);

String operator+(const String& lhs, const String& rhs);

String operator+(const String& lhs, StringView rhs);

String operator+(const String& lhs, const char* rhs);

String operator+(StringView lhs, const String& rhs);

String operator+(const char* lhs, const String& rhs);

String operator*(const String& str, u32 repeatCount);

String operator*(u32 repeatCount, const String& str);

String operator*(StringView str, u32 repeatCount);

String operator*(u32 repeatCount, StringView str);
} // namespace Polly

inline Polly::String operator""_s(const char* str, size_t length)
{
    return Polly::String(str, static_cast<Polly::u32>(length));
}

#include "Polly/Details/String.inl"
