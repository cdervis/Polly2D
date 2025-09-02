// Copyright (C) 2025 Cem Dervis
// This file is part of Polly, a 2D C++ game framework for minimalists.
// For conditions of distribution and use, see copyright notice in LICENSE, or https://polly2d.org.

#pragma once

#include "Polly/Assume.hpp"
#include "Polly/Math.hpp"
#include "Polly/Maybe.hpp"
#include <algorithm>
#include <cstring>
#include <functional>

namespace Polly
{
namespace Details
{
static constexpr u32 string_literal_length(const char* str)
{
#ifdef __cpp_if_consteval
    if not consteval
    {
#else
    if (not std::is_constant_evaluated())
    {
#endif
        assumeWithMsg(str, "string literal must not be null");
    }

    auto size = 0u;

    for (size = 0; *str != '\0'; ++str)
    {
        ++size;
    }

    return size;
}

[[noreturn]]
void throw_empty_string_view_exception();

[[noreturn]]
void throw_string_view_out_of_range_exception();

[[noreturn]]
void throw_string_view_range_exception();

[[noreturn]]
void throw_string_view_not_null_terminated_exception();
} // namespace Details

constexpr StringView::StringView()
    : _data("")
    , _size(0)
    , _isNullTerminated(true)
{
}

constexpr StringView::StringView(Details::NoObjectTag)
    : StringView()
{
}

constexpr StringView::StringView(const char* str)
    : _data(str)
    , _size(Details::string_literal_length(str))
    , _isNullTerminated(false)
{
}

constexpr StringView::StringView(const char* str, u32 size, bool isNullTerminated)
    : _data(str)
    , _size(size)
    , _isNullTerminated(isNullTerminated)
{
}

constexpr StringView StringView::substring(u32 offset) const
{
    return substring(offset, {});
}

constexpr StringView StringView::substring(u32 offset, Maybe<u32> size) const
{
    if (size)
    {
        if (offset + *size >= _size)
        {
            Details::throw_string_view_range_exception();
        }

        return StringView(_data + offset, *size);
    }

    if (offset > _size)
    {
        Details::throw_string_view_out_of_range_exception();
    }

    return StringView(_data + offset, _size - offset);
}

// ReSharper disable once CppNonExplicitConversionOperator
constexpr StringView::operator Span<char>() const
{
    return Span(_data, _size);
}

inline bool StringView::contains(StringView str) const
{
    return static_cast<bool>(find(str));
}

inline bool StringView::contains(char ch) const
{
    return static_cast<bool>(find(ch));
}

inline u32 StringView::count(char ch) const
{
    auto result = static_cast<u32>(0);

    for (const auto item : *this)
    {
        if (item == ch)
        {
            ++result;
        }
    }

    return result;
}

inline u32 StringView::count(StringView str) const
{
    auto result = static_cast<u32>(0);
    auto idx    = find(str);

    while (idx)
    {
        ++result;
        idx = find(str, *idx + str.size());
    }

    return result;
}

constexpr bool StringView::isNullTerminated() const
{
    return _isNullTerminated;
}

#if __cpp_multidimensional_subscript >= 202110L
constexpr auto StringView::operator[](u32 offset, u32 size) const -> StringView
{
    return substring(offset, size);
}
#endif

constexpr const char* StringView::cstring() const
{
    // Always do this check, regardless of hardening.
    // C-strings are mostly passed to external C functions, which may
    // do things we really don't like when the string isn't null-terminated.
    if (not _data or not _isNullTerminated)
    {
        Details::throw_string_view_not_null_terminated_exception();
    }

    return _data;
}

constexpr const char* StringView::data() const
{
    return _data;
}

constexpr u32 StringView::size() const
{
    return _size;
}

constexpr bool StringView::isEmpty() const
{
    return _size == 0;
}

constexpr const char* StringView::begin() const
{
    return _data;
}

constexpr const char* StringView::end() const
{
    return _data + _size;
}

constexpr const char& StringView::operator[](u32 index) const
{
    if (index > _size)
    {
        Details::throw_string_view_out_of_range_exception();
    }

    return _data[index];
}

constexpr const char& StringView::first() const
{
    if (isEmpty())
    {
        Details::throw_empty_string_view_exception();
    }

    return _data[0];
}

constexpr const char& StringView::last() const
{
    if (isEmpty())
    {
        Details::throw_empty_string_view_exception();
    }

    return _data[_size - 1];
}


inline Maybe<u32> StringView::find(char ch, u32 offset) const
{
    if (offset >= _size)
    {
        Details::throw_string_view_range_exception();
    }

    const auto  my_size = _size;
    const auto* my_data = _data + offset;

    for (auto i = offset; i < my_size; ++i, ++my_data)
    {
        if (*my_data == ch)
        {
            return i;
        }
    }

    return none;
}

template<typename Predicate>
requires(std::invocable<Predicate, char>)
constexpr bool StringView::all(Predicate&& predicate) const
{
    const auto* data = _data;
    const auto  sz   = _size;

    for (u32 i = 0u; i < sz; ++i)
    {
        if (not std::invoke(predicate, data[i]))
        {
            return false;
        }
    }

    return true;
}

inline bool operator<(const StringView& lhs, const StringView& rhs)
{
    const auto lhs_size         = lhs.size();
    const auto rhs_size         = rhs.size();
    const auto chars_to_compare = min(lhs_size, rhs_size);
    const auto cmp              = std::strncmp(lhs.data(), rhs.data(), chars_to_compare);

    if (cmp < 0)
    {
        return true;
    }

    if (cmp > 0)
    {
        return false;
    }

    return lhs_size < rhs_size;
}

inline bool operator<=(const StringView& lhs, const StringView& rhs)
{
    const auto lhs_size         = lhs.size();
    const auto rhs_size         = rhs.size();
    const auto chars_to_compare = min(lhs_size, rhs_size);
    const auto cmp              = std::strncmp(lhs.data(), rhs.data(), chars_to_compare);

    if (cmp < 0)
    {
        return true;
    }

    if (cmp > 0)
    {
        return false;
    }

    return lhs_size <= rhs_size;
}

inline bool operator>(const StringView& lhs, const StringView& rhs)
{
    const auto lhs_size         = lhs.size();
    const auto rhs_size         = rhs.size();
    const auto chars_to_compare = min(lhs_size, rhs_size);
    const auto cmp              = std::strncmp(lhs.data(), rhs.data(), chars_to_compare);

    if (cmp < 0)
    {
        return false;
    }

    if (cmp > 0)
    {
        return true;
    }

    return lhs_size > rhs_size;
}

inline bool operator>=(const StringView& lhs, const StringView& rhs)
{
    const auto lhs_size         = lhs.size();
    const auto rhs_size         = rhs.size();
    const auto chars_to_compare = min(lhs_size, rhs_size);
    const auto cmp              = std::strncmp(lhs.data(), rhs.data(), chars_to_compare);

    if (cmp < 0)
    {
        return false;
    }

    if (cmp > 0)
    {
        return true;
    }

    return lhs_size >= rhs_size;
}

inline bool operator==(const StringView& lhs, const StringView& rhs)
{
    const auto lhs_size = lhs.size();

    if (lhs_size != rhs.size())
    {
        return false;
    }

    return std::strncmp(lhs.data(), rhs.data(), lhs_size) == 0;
}

inline bool operator==(const StringView& lhs, const char* rhs)
{
    return lhs == StringView(rhs);
}

inline bool operator!=(const StringView& lhs, const StringView& rhs)
{
    return not(lhs == rhs);
}

inline auto operator<=>(const StringView& lhs, const StringView& rhs)
{
    return std::lexicographical_compare_three_way(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}
} // namespace Polly
