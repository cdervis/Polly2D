// Copyright (C) 2023-2024 Cem Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE.

#include "Polly/String.hpp"
#include "fast_float.hpp"
#include "Polly/Array.hpp"
#include "Polly/Core/to_chars.hpp"
#include "Polly/Logging.hpp"
#include "Polly/Maybe.hpp"
#include "Polly/Narrow.hpp"

namespace Polly
{
String String::repeat(char ch, u32 count)
{
    auto result = String();
    result.ensureSize(count);

    auto* data = result.data();

    for (auto i = 0u; i < count; ++i)
    {
        data[i] = ch;
    }

    return result;
}

String String::repeat(StringView str, u32 count, StringView separator)
{
    auto result = String();
    result.reserve((str.size() + separator.size()) * count);

    for (u32 i = 0; i < count; ++i)
    {
        result += str;

        if (i + 1 < count)
        {
            result += separator;
        }
    }

    return result;
}

char* String::rawConcat(char* dst, const char* src)
{
    return std::strcat(dst, src);
}

void String::assign(const char* str, Maybe<u32> size)
{
    if (not size)
    {
        size = narrow<u32>(std::strlen(str));
    }

    reserve(*size);
    auto* dstPtr = data();
    std::memcpy(dstPtr, str, *size * sizeof(char));
    dstPtr[*size] = '\0';
    _size         = *size;
}

void String::clear()
{
    _size    = 0;
    _data[0] = '\0';
}

void String::reserve(u32 newCapacity)
{
    if (_capacity >= newCapacity)
    {
        return;
    }

    const auto currentSize = size();

    auto* newBuffer = new char[newCapacity + 1];

    if (_size > 0)
    {
        std::memcpy(newBuffer, data(), currentSize);
        newBuffer[currentSize] = '\0';
    }

    if (not isSmall())
    {
        delete[] _data;
    }

    _data     = newBuffer;
    _capacity = newCapacity;
}

void String::ensureSize(u32 newSize)
{
    const auto currentSize = size();

    if (currentSize >= newSize)
    {
        return;
    }

    reserve(newSize);

    const auto previousSize = _size;
    _size                   = newSize;

    std::fill(_data + previousSize, _data + _size, '\0');
}

void String::trim(SmallList<char> chars)
{
    const auto shouldRemove = [&chars](char ch)
    {
        return std::ranges::find(chars, ch) != chars.end();
    };

    while (not isEmpty() and shouldRemove(last()))
    {
        removeLast();
    }

    while (not isEmpty() and shouldRemove(first()))
    {
        removeFirst();
    }
}

void String::trimStart(SmallList<char> chars)
{
    auto trimmed = StringView(*this);
    trimmed.trimStart(chars);

    assume(trimmed.size() <= size());

    removeFirst(size() - trimmed.size());
}

void String::trimEnd(SmallList<char> chars)
{
    auto trimmed = StringView(*this);
    trimmed.trimEnd(chars);

    assume(trimmed.data() >= begin() and trimmed.data() < end()); // NOLINT(*-simplify-boolean-expr)

    removeLast(size() - trimmed.size());
}

String String::trimmed(SmallList<char> chars) const
{
    auto result = String{*this};
    result.trim(chars);

    return result;
}

void String::transformToLowerCase()
{
    for (auto& ch : *this)
    {
        ch = static_cast<char>(std::tolower(ch));
    }
}

String String::lowerCased() const
{
    return StringView(*this).lowerCased();
}

void String::transformToUpperCase()
{
    for (auto& ch : *this)
    {
        ch = static_cast<char>(std::toupper(ch));
    }
}

String String::upperCased() const
{
    return StringView(*this).upperCased();
}

void String::replaceCharacter(char oldChar, char newChar)
{
    for (auto& ch : *this)
    {
        if (ch == oldChar)
        {
            ch = newChar;
        }
    }
}

char& String::first()
{
    checkAccess();
    return _data[0];
}

const char& String::first() const
{
    checkAccess();
    return _data[0];
}

char& String::last()
{
    checkAccess();
    return _data[_size - 1];
}

const char& String::last() const
{
    checkAccess();
    return _data[_size - 1];
}

void String::remove(u32 index)
{
    remove(index, {});
}

void String::remove(u32 index, Maybe<u32> size)
{
    if (size)
    {
        // TODO: this can be optimized
        const auto len = *size;

        auto*       dst  = _data + index;
        const auto* src  = dst + len;
        const auto  diff = _size - (index + len);

        for (u32 i = 0u; i < diff; ++i)
        {
            dst[i] = src[i];
        }

        _size -= len;
        _data[_size] = '\0';
    }
    else
    {
        // Optimized version where we just trim the string at 'index'.
        const auto removedChars = _size - index;
        _data[index]            = '\0';
        _size -= removedChars;
    }
}

Maybe<int> String::toInt() const
{
    return StringView(*this).toInt();
}

Maybe<unsigned int> String::toUInt() const
{
    return StringView(*this).toUInt();
}

Maybe<float> String::toFloat() const
{
    return StringView(*this).toFloat();
}

Maybe<double> String::toDouble() const
{
    return StringView(*this).toDouble();
}

void String::insertAt(u32 index, StringView str)
{
    if (isEmpty() and index == 0)
    {
        assign(str.data(), str.size());
        return;
    }

    ensureSize(_size + str.size());

    // TODO: this can be optimized
    auto* p = _data + index;
    std::memmove(p + str.size(), p, _size - index);
    std::memcpy(p, str.data(), str.size());
}

void String::append(StringView str)
{
    if (str.isEmpty())
    {
        return;
    }

    const auto newSize = size() + str.size();

    reserve(newSize);

    std::memmove(_data + _size, str.data(), str.size() * sizeof(char));

    _size        = newSize;
    _data[_size] = '\0';
}

void String::checkAccess() const
{
#ifndef polly_no_hardening
    if (isEmpty())
    {
        Details::throw_empty_string_exception();
    }
#endif
}

void String::checkAccessAt(u32 idx) const
{
#ifndef polly_no_hardening
    if (idx >= size())
    {
        Details::throw_string_out_of_range_exception();
    }
#endif
}

void String::moveFrom(String&& other) // NOLINT(*-rvalue-reference-param-not-moved)
{
    const auto otherSmall = other.isSmall();

    // We can retain our large buffer if we have one, and if the other string
    // is a small string. We just copy its contents into our large buffer.
    const auto shouldRetainLargeBuffer = not isSmall() and otherSmall;

    if (not shouldRetainLargeBuffer)
    {
        destroy();
    }

    _size = other._size;

    if (not shouldRetainLargeBuffer)
    {
        _capacity = other._capacity;
    }

    if (not otherSmall)
    {
        // Obtain the other string's large buffer pointer.
        _data = other._data;
    }
    else if (_capacity < smallBufferSize - 1)
    {
        // The other string was not large, which means we're not large either
        // after this move operation. Therefore point our data to our small buffer.
        _data = &_smallBuffer[0];
    }

    if (_data != other._data)
    {
        std::memcpy(_data, other._smallBuffer, sizeof(_smallBuffer));
    }

    other._size     = 0;
    other._capacity = smallBufferSize - 1;
    other._data     = &other._smallBuffer[0];
    other._data[0]  = '\0';
}

template<typename T, size_t BufferSize>
static String toString(T value)
{
    auto  buffer = Array<char, BufferSize>();
    auto* begin  = buffer.begin();

    const auto* ptr = simdjson::internal::to_chars(
        begin,
        buffer.end(),
        static_cast<double>(value),
        std::is_integral_v<std::remove_cvref_t<T>>);

    return String(begin, static_cast<u32>(ptr - begin));
}

String toString(short value)
{
    return toString<short, 20>(value);
}

String toString(unsigned short value)
{
    return toString<unsigned short, 20>(value);
}

String toString(int value)
{
    return toString<int, 20>(value);
}

String toString(unsigned int value)
{
    return toString<unsigned int, 20>(value);
}

String toString(long value)
{
    return toString<long, 30>(value);
}

String toString(unsigned long value)
{
    return toString<unsigned long, 30>(value);
}

String toString(long long value)
{
    return toString<long long, 30>(value);
}

String toString(unsigned long long value)
{
    return toString<unsigned long long, 40>(value);
}

String toString(float value)
{
    return toString<float, 20>(value);
}

String toString(double value)
{
    return toString<double, 40>(value);
}

String toString(bool value)
{
    return value ? "true" : "false";
}
} // namespace Polly
