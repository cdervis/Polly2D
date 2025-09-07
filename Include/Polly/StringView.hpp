// Copyright (C) 2023-2024 Cem Dervis
// This file is part of Polly, a 2D C++ game framework for minimalists.
// For conditions of distribution and use, see copyright notice in LICENSE, or https://polly2d.org.

#pragma once

#include "Polly/List.hpp"
#include "Polly/Prerequisites.hpp"
#include "Polly/Span.hpp"

namespace Polly
{
template<typename T>
class Maybe;

class String;

class StringView
{
  public:
    using value_type                            = char;
    static constexpr auto isContiguousContainer = true;
    static constexpr auto isForwardContainer    = true;

    constexpr StringView();

    constexpr StringView(Details::NoObjectTag);

    constexpr StringView(const char* str);

    constexpr StringView(const char* str, u32 size, bool isNullTerminated = false);

    template<size_t N>
    constexpr StringView(const char (&str)[N])
        : _data(str)
        , _size(N)
        , _isNullTerminated(true)
    {
    }

    StringView(std::nullptr_t) =
        PollyDeleteWithReason("Constructing a StringView from a nullptr is invalid.");

    constexpr StringView substring(u32 offset) const;

    constexpr StringView substring(u32 offset, Maybe<u32> size) const;

    Maybe<u32> find(StringView str, u32 offset = 0) const;

    Maybe<u32> find(char ch, u32 offset = 0) const;

    bool contains(StringView str) const;

    bool contains(char ch) const;

    u32 count(char ch) const;

    u32 count(StringView str) const;

    Maybe<u32> findAnyOf(StringView chars) const;

    Maybe<u32> findAnyOf(StringView chars, Maybe<u32> offset) const;

    Maybe<u32> reverseFind(StringView str) const;

    Maybe<u32> reverseFind(StringView str, Maybe<u32> offset) const;

    Maybe<u32> reverseFind(char ch) const;

    Maybe<u32> reverseFind(char ch, Maybe<u32> offset) const;

    bool startsWith(StringView str) const;

    bool endsWith(StringView str) const;

    template<typename Predicate>
    requires(std::invocable<Predicate, char>)
    constexpr bool all(Predicate&& predicate) const;

    void removeFirst(u32 count = 1);

    void removeLast(u32 count = 1);

    void trim(SmallList<char> chars = {' '});

    void trimStart(SmallList<char> chars = {' '});

    void trimEnd(SmallList<char> chars = {' '});

    [[nodiscard]]
    StringView trimmed(SmallList<char> chars = {' '}) const;

    [[nodiscard]]
    String lowerCased() const;

    [[nodiscard]]
    String upperCased() const;

    constexpr const char* data() const;

    constexpr u32 size() const;

    constexpr bool isEmpty() const;

    constexpr const char* begin() const;

    constexpr const char* end() const;

    constexpr const char& operator[](u32 index) const;

    constexpr const char& first() const;

    constexpr const char& last() const;

    Maybe<int> toInt() const;

    Maybe<u32> toUInt() const;

    Maybe<float> toFloat() const;

    Maybe<double> toDouble() const;

    size_t hashCode() const;

    constexpr operator Span<char>() const;

    constexpr bool isNullTerminated() const;

    constexpr const char* cstring() const;

#if __cpp_multidimensional_subscript >= 202110L
    constexpr auto operator[](u32 offset, u32 size) const -> StringView;
#endif

  private:
    const char* _data;
    u32         _size;
    bool        _isNullTerminated;
};

String operator+(const StringView& lhs, const StringView& rhs);

bool operator<(const StringView& lhs, const StringView& rhs);

bool operator<=(const StringView& lhs, const StringView& rhs);

bool operator>(const StringView& lhs, const StringView& rhs);

bool operator>=(const StringView& lhs, const StringView& rhs);

bool operator==(const StringView& lhs, const StringView& rhs);

bool operator==(const StringView& lhs, const char* rhs);

bool operator!=(const StringView& lhs, const StringView& rhs);

auto operator<=>(const StringView& lhs, const StringView& rhs);
} // namespace Polly

constexpr Polly::StringView operator""_sv(const char* str, size_t length)
{
    return Polly::StringView(str, static_cast<Polly::u32>(length), true);
}

#include "Polly/Details/StringView.inl"
