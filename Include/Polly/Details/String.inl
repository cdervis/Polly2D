// Copyright (C) 2025 Cem Dervis
// This file is part of Polly, a minimalistic 2D C++ game framework.
// For conditions of distribution and use, see copyright notice in LICENSE, or https://polly2d.org.

#pragma once

#include "Polly/Error.hpp"
#include "Polly/Maybe.hpp"

namespace Polly
{
namespace Details
{
inline void throw_empty_string_exception()
{
    throw Error("Attempting to access characters of an empty String.");
}

inline void throw_string_out_of_range_exception()
{
    throw Error("Index out of String range.");
}
} // namespace Details

constexpr String::String()
    : _data(&_smallBuffer[0])
{
}

constexpr String::String(Details::NoObjectTag)
    : String()
{
}

inline String::String(const char* str)
    : String()
{
    assign(str, rawLength(str));
}

inline String::String(const char* str, uint32_t size)
    : String()
{
    assign(str, size);
}

inline String::String(const String& other)
    : String()
{
    assign(other.data(), other.size());
}

inline String::String(StringView view)
    : String()
{
    assign(view.data(), view.size());
}

inline String& String::operator=(const String& other)
{
    if (this != &other)
    {
        assign(other.data(), other.size());
    }

    return *this;
}

inline String& String::operator=(StringView other)
{
    assign(other.data(), other.size());
    return *this;
}

inline String& String::operator=(const char* other)
{
    assign(other, Details::string_literal_length(other));
    return *this;
}

inline String::String(String&& other) noexcept
    : String()
{
    moveFrom(std::move(other));
}

inline String& String::operator=(String&& other) noexcept
{
    if (this != &other)
    {
        moveFrom(std::move(other));
    }

    return *this;
}

inline String::~String() noexcept
{
    destroy();
}

inline void String::removeFirst(u32 count)
{
    if (count == 0)
    {
        return;
    }

    checkAccess();

    remove(0, count);
}

inline void String::removeLast(u32 count)
{
    if (count == 0)
    {
        return;
    }

    checkAccess();

    remove(_size - count);
}

inline String String::substring(uint32_t offset) const
{
    return substring(offset, none);
}

inline String String::substring(uint32_t offset, Maybe<uint32_t> size) const
{
    return String(StringView(*this).substring(offset, size));
}

inline Maybe<uint32_t> String::find(StringView str, uint32_t offset) const
{
    return StringView(*this).find(str, offset);
}

inline Maybe<uint32_t> String::find(char ch, uint32_t offset) const
{
    return StringView(*this).find(ch, offset);
}

inline Maybe<uint32_t> String::findAnyOf(StringView chars, uint32_t offset) const
{
    return StringView(*this).findAnyOf(chars, offset);
}

inline Maybe<uint32_t> String::reverseFind(StringView str) const
{
    return reverseFind(str, {});
}

inline Maybe<uint32_t> String::reverseFind(StringView str, Maybe<uint32_t> offset) const
{
    return StringView(*this).reverseFind(str, offset);
}

inline Maybe<uint32_t> String::reverseFind(char ch) const
{
    return reverseFind(ch, {});
}

inline Maybe<uint32_t> String::reverseFind(char ch, Maybe<uint32_t> offset) const
{
    return reverseFind(StringView{&ch, 1}, offset);
}

inline bool String::startsWith(StringView str) const
{
    return StringView(*this).startsWith(str);
}

inline bool String::endsWith(StringView str) const
{
    return StringView(*this).endsWith(str);
}

inline bool String::contains(StringView str) const
{
    return bool(find(str));
}

inline bool String::contains(char ch) const
{
    return bool(find(ch));
}

inline u32 String::count(char ch) const
{
    return StringView(*this).count(ch);
}

inline String::operator StringView() const
{
    return StringView(_data, _size, true);
}

inline char* String::data()
{
    return _data;
}

inline const char* String::data() const
{
    return _data;
}

inline const char* String::cstring() const
{
    return _data;
}

inline char* String::begin()
{
    return _data;
}

inline const char* String::begin() const
{
    return _data;
}

inline char* String::end()
{
    return _data + _size;
}

inline const char* String::end() const
{
    return _data + _size;
}

inline uint32_t String::size() const
{
    return _size;
}

inline bool String::isEmpty() const
{
    return _size == 0;
}

inline uint32_t String::capacity() const
{
    return _capacity;
}

inline bool String::isSmall() const
{
    return _data == &_smallBuffer[0];
}

inline size_t String::hashCode() const
{
    return StringView(*this).hashCode();
}

inline char& String::operator[](uint32_t index)
{
#ifndef polly_no_hardening
    if (index >= _size)
    {
        Details::throw_string_out_of_range_exception();
    }
#endif

    return _data[index];
}

inline const char& String::operator[](uint32_t index) const
{
#ifndef polly_no_hardening
    if (index >= _size)
    {
        Details::throw_string_out_of_range_exception();
    }
#endif

    return _data[index];
}

inline void String::operator+=(const String& other)
{
    append(StringView(other));
}

inline void String::operator+=(StringView other)
{
    append(other);
}

inline void String::operator+=(const char* other)
{
    append(StringView{other});
}

inline void String::operator+=(char ch)
{
    append(StringView(&ch, 1));
}

constexpr uint32_t String::rawLength(const char* str)
{
    return Details::string_literal_length(str);
}

inline void String::destroy()
{
    if (not isSmall())
    {
        delete[] _data;
        _data = &_smallBuffer[0];
    }
}

inline bool operator<(const String& lhs, const String& rhs)
{
    return StringView(lhs) < StringView(rhs);
}

inline bool operator<(const String& lhs, const StringView& rhs)
{
    return StringView(lhs) < rhs;
}

inline bool operator<=(const String& lhs, const String& rhs)
{
    return StringView(lhs) <= StringView(rhs);
}

inline bool operator<=(const String& lhs, const StringView& rhs)
{
    return StringView(lhs) <= rhs;
}

inline bool operator>(const String& lhs, const String& rhs)
{
    return StringView(lhs) > StringView(rhs);
}

inline bool operator>(const String& lhs, const StringView& rhs)
{
    return StringView(lhs) > rhs;
}

inline bool operator>=(const String& lhs, const String& rhs)
{
    return StringView(lhs) >= StringView(rhs);
}

inline bool operator>=(const String& lhs, const StringView& rhs)
{
    return StringView(lhs) >= rhs;
}

inline bool operator==(const String& lhs, const String& rhs)
{
    return StringView(lhs) == StringView(rhs);
}

inline bool operator==(const String& lhs, StringView rhs)
{
    return StringView(lhs) == rhs;
}

inline bool operator==(StringView lhs, const String& rhs)
{
    return lhs == StringView(rhs);
}

inline bool operator==(const String& lhs, const char* rhs)
{
    return StringView(lhs) == StringView(rhs);
}

inline bool operator!=(const String& lhs, const String& rhs)
{
    return not(lhs == rhs);
}

inline bool operator!=(const String& lhs, StringView rhs)
{
    return not(lhs == rhs);
}

inline bool operator!=(StringView lhs, const String& rhs)
{
    return not(lhs == rhs);
}

inline String operator+(const String& lhs, const String& rhs)
{
    auto result = String();
    result.reserve(lhs.size() + rhs.size());
    result += lhs;
    result += rhs;
    return result;
}

inline String operator+(const String& lhs, StringView rhs)
{
    auto result = String();
    result.reserve(lhs.size() + rhs.size());
    result += lhs;
    result += rhs;
    return result;
}

inline String operator+(StringView lhs, const String& rhs)
{
    auto result = String();
    result.reserve(lhs.size() + rhs.size());
    result += lhs;
    result += rhs;
    return result;
}

inline String operator+(const char* lhs, const String& rhs)
{
    return StringView(lhs) + rhs;
}

inline String operator+(const String& lhs, const char* rhs)
{
    return lhs + StringView(rhs);
}

inline String operator*(const String& str, u32 repeatCount)
{
    return String::repeat(str, repeatCount);
}

inline String operator*(u32 repeatCount, const String& str)
{
    return String::repeat(str, repeatCount);
}

inline String operator*(StringView str, u32 repeatCount)
{
    return String::repeat(str, repeatCount);
}

inline String operator*(u32 repeatCount, StringView str)
{
    return String::repeat(str, repeatCount);
}
} // namespace Polly