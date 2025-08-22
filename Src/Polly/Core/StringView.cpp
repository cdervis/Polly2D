// Copyright (C) 2023-2024 Cemalettin Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE.

#include "Polly/StringView.hpp"
#include "fast_float.hpp"
#include "komihash.h"
#include "Polly/Algorithm.hpp"
#include "Polly/Error.hpp"
#include "Polly/Logging.hpp"
#include "Polly/Math.hpp"
#include "Polly/Maybe.hpp"
#include <SDL3/SDL_stdinc.h>

namespace Polly
{
Maybe<u32> StringView::find(StringView str, u32 offset) const
{
    const auto needleSize = str.size();
    const auto mySize     = _size;

    if (needleSize == 0 or offset >= mySize)
    {
        return {};
    }

    const auto  needleData = str._data;
    const auto  firstCh    = needleData[0];
    const auto* myData     = _data;
    const char* myBegin    = myData + offset;
    const auto* myEnd      = myData + mySize;

    for (auto size = mySize - offset; size >= needleSize; size = static_cast<int>(myEnd - myBegin))
    {
        const auto remainingSpace = size - needleSize + 1;

        myBegin = static_cast<const char*>(std::memchr(myBegin, firstCh, remainingSpace));

        if (not myBegin)
        {
            return none;
        }

        if (std::memcmp(myBegin, needleData, needleSize) == 0)
        {
            return static_cast<int>(myBegin - myData);
        }

        ++myBegin;
    }

    return none;
}

Maybe<u32> StringView::findAnyOf(StringView chars) const
{
    return findAnyOf(chars, none);
}

Maybe<u32> StringView::findAnyOf(StringView chars, Maybe<u32> offset) const
{
    offset = offset.valueOr(0);

    if (*offset >= _size)
    {
        Details::throw_string_view_range_exception();
    }

    const auto  mySize = _size;
    const auto* myData = _data + *offset;

    for (auto i = *offset; i < mySize; ++i, ++myData)
    {
        for (const auto ch : chars)
        {
            if (*myData == ch)
            {
                return i;
            }
        }
    }

    return none;
}

Maybe<u32> StringView::reverseFind(StringView str) const
{
    return reverseFind(str, none);
}

Maybe<u32> StringView::reverseFind(StringView str, Maybe<u32> offset) const
{
    const auto* needleData = str._data;
    const auto  needleSize = str._size;
    const auto* myData     = _data;
    const auto  mySize     = _size;
    const auto  diff       = mySize - needleSize;

    if (not offset)
    {
        offset = std::numeric_limits<u32>::max();
    }

    if (needleSize <= mySize)
    {
        offset = Polly::min<u32>(diff, *offset);

        const auto search = [&]
        {
            return std::memcmp(myData + *offset, needleData, needleSize) == 0;
        };

        if (search())
        {
            return offset;
        }

        while (*offset > 0)
        {
            if (search())
            {
                return offset;
            }

            --*offset;

            if (*offset == 0)
            {
                break;
            }
        }
    }

    return none;
}

Maybe<u32> StringView::reverseFind(char ch) const
{
    return reverseFind(ch, {});
}

Maybe<u32> StringView::reverseFind(char ch, Maybe<u32> offset) const
{
    // TODO: this can be optimized
    return reverseFind(StringView(&ch, 1), offset);
}

bool StringView::startsWith(StringView str) const
{
    const auto idx = find(str);
    return idx and *idx == 0;
}

bool StringView::endsWith(StringView str) const
{
    const auto index = reverseFind(str);
    return index ? *index + str.size() == size() : false;
}

void StringView::removeFirst(u32 count)
{
    if (count > _size)
    {
        throw Error("Invalid count specified.");
    }

    assume(_size >= count);

    _size -= count;
    _data += count;
}

void StringView::removeLast(u32 count)
{
    if (count > _size)
    {
        throw Error("Invalid count specified.");
    }

    assume(_size >= count);

    _size -= count;
}

void StringView::trim(SmallList<char> chars)
{
    trimEnd(chars);
    trimStart(chars);
}

void StringView::trimStart(SmallList<char> chars)
{
    const auto* const endPtr = end();

    while (_data != endPtr and Polly::find(chars, *_data))
    {
        ++_data, --_size;
    }
}

void StringView::trimEnd(SmallList<char> chars)
{
    const auto* const beginPtr = begin();
    const auto* const endPtr   = end() - 1;
    auto*             last     = endPtr;

    while (last != beginPtr and Polly::find(chars, *last))
    {
        --last;
    }

    if (last < endPtr)
    {
        _size -= static_cast<u32>(endPtr - last);
        _isNullTerminated = false;
    }
}

StringView StringView::trimmed(SmallList<char> chars) const
{
    auto result = StringView(*this);
    result.trim(chars);
    return result;
}

String StringView::lowerCased() const
{
    auto result = String{*this};

    for (auto& ch : result)
    {
        ch = static_cast<char>(SDL_tolower(ch));
    }

    return result;
}

String StringView::upperCased() const
{
    auto result = String{*this};

    for (auto& ch : result)
    {
        ch = static_cast<char>(SDL_toupper(ch));
    }

    return result;
}

template<typename T>
static Maybe<T> convertStringTo(StringView str)
{
    auto value         = T();
    const auto [_, ec] = fast_float::from_chars(str.begin(), str.end(), value);
    return ec != std::errc() ? Maybe<T>() : value;
}

Maybe<int> StringView::toInt() const
{
    return convertStringTo<int>(*this);
}

Maybe<u32> StringView::toUInt() const
{
    return convertStringTo<unsigned int>(*this);
}

Maybe<float> StringView::toFloat() const
{
    return convertStringTo<float>(*this);
}

Maybe<double> StringView::toDouble() const
{
    return convertStringTo<double>(*this);
}

size_t StringView::hashCode() const
{
    return komihash(_data, _size, 31);
}
} // namespace Polly
