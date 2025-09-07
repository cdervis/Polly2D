// Copyright (C) 2025 Cem Dervis
// This file is part of Polly, a 2D C++ game framework for minimalists.
// For conditions of distribution and use, see copyright notice in LICENSE, or https://polly2d.org.

#pragma once

#include "Polly/Assume.hpp"
#include "Polly/Concepts.hpp"
#include "Polly/Maybe.hpp"
#include "Polly/Prerequisites.hpp"
#include "Polly/Random.hpp"
#include "Polly/ToString.hpp"
#include <algorithm>

namespace Polly
{
class StringView;

template<Concepts::ForwardContainer Container, typename T>
static constexpr Maybe<u32> indexOf(const Container& container, const T& value)
{
    for (auto idx = 0u; const auto& item : container)
    {
        if (item == value)
        {
            return idx;
        }

        ++idx;
    }

    return none;
}

template<Concepts::ForwardContainer Container, typename Predicate>
constexpr Maybe<u32> indexOfWhere(const Container& container, Predicate&& predicate)
{
    for (auto idx = 0u; const auto& item : container)
    {
        if (std::invoke(predicate, item))
        {
            return idx;
        }

        ++idx;
    }

    return none;
}

template<Concepts::ForwardContainer Container, typename T>
constexpr auto find(Container&& container, const T& value)
{
    using ItemType = std::add_lvalue_reference_t<decltype(*container.begin())>;

    for (auto&& item : container)
    {
        if (item == value)
        {
            return Maybe<ItemType>(item);
        }
    }

    return Maybe<ItemType>();
}

template<Concepts::ContiguousContainer Container, typename T>
constexpr auto reverseFind(Container&& container, const T& value)
{
    using ItemType = std::add_lvalue_reference_t<decltype(*container.begin())>;

    const auto size = container.size();

    for (auto i = static_cast<ptrdiff_t>(size) - 1; i >= 0; --i)
    {
        if (container[i] == value)
        {
            return Maybe<ItemType>(container[i]);
        }
    }

    return Maybe<ItemType>();
}

template<Concepts::ForwardContainer Container, typename Predicate>
constexpr auto findWhere(Container&& container, Predicate&& predicate)
{
    using ItemType = std::add_lvalue_reference_t<decltype(*container.begin())>;

    for (auto&& item : container)
    {
        if (std::invoke(predicate, item))
        {
            return Maybe<ItemType>(item);
        }
    }

    return Maybe<ItemType>();
}

template<Concepts::ForwardContainer Container, typename Predicate>
constexpr bool all(const Container& container, Predicate&& predicate)
{
    if (container.isEmpty())
    {
        return true;
    }

    using ItemType = typename Container::value_type;

    const auto size = container.size();

    if constexpr (std::invocable<Predicate, const ItemType&, u32>)
    {
        for (u32 i = 0u; i < size; ++i)
        {
            if (not std::invoke(predicate, container[i], i))
            {
                return false;
            }
        }
    }
    else
    {
        for (u32 i = 0u; i < size; ++i)
        {
            if (not std::invoke(predicate, container[i]))
            {
                return false;
            }
        }
    }

    return true;
}

template<Concepts::ForwardContainer T, typename U>
constexpr bool contains(const T& container, const U& value)
{
    return bool(indexOf(container, value));
}

template<Concepts::ForwardContainer Container, typename U>
constexpr u32 count(const Container& container, const U& value)
{
    auto count = 0u;

    for (const auto& valueInContainer : container)
    {
        if (valueInContainer == value)
        {
            ++count;
        }
    }

    return count;
}

template<Concepts::ForwardContainer Container, typename Predicate>
constexpr u32 countWhere(const Container& container, Predicate&& predicate)
{
    auto count = u32(0);

    for (const auto& valueInContainer : container)
    {
        if (std::invoke(predicate, valueInContainer))
        {
            ++count;
        }
    }

    return count;
}

template<Concepts::ForwardContainer Container, typename Predicate>
constexpr bool containsWhere(const Container& container, Predicate&& predicate)
{
    for (const auto& valueInContainer : container)
    {
        if (std::invoke(predicate, valueInContainer))
        {
            return true;
        }
    }

    return false;
}

template<Concepts::ForwardContainer Container1, Concepts::ForwardContainer Container2>
[[nodiscard]]
constexpr bool areContainersEqual(const Container1& container1, const Container2& container2)
{
    const auto lhsSize = container1.size();
    const auto rhsSize = container2.size();

    if (lhsSize != rhsSize)
    {
        return false;
    }

    auto lhsIt = container1.begin();
    auto rhsIt = container2.begin();

    for (u32 i = 0; i < lhsSize; ++i)
    {
        if (*lhsIt != *rhsIt)
        {
            return false;
        }

        ++lhsIt;
        ++rhsIt;
    }

    return true;
}

template<typename SrcContainer, Concepts::ContiguousContainer DstContainer>
constexpr void copyRange(const SrcContainer& from, const DstContainer& to, u32 dstStartIndex = 0)
{
    assumeWithMsg(
        dstStartIndex + from.size() <= to.size(),
        "specified source container + dstStartIndex would exceed the destination container's "
        "bounds");

    for (const auto& element : from)
    {
        to[dstStartIndex++] = element;
    }
}

/// Performs a binary search over a range.
template<Concepts::ContiguousContainer Container, typename T>
constexpr Maybe<u32> binaryFindIndex(const Container& container, const T& value)
{
    auto count = container.size();
    auto step  = u32(0);
    auto it    = typename Container::const_iterator();
    auto first = container.begin();

    while (count > 0)
    {
        it   = first;
        step = count / 2;
        it += step;

        if (*it < value)
        {
            first = ++it;
            count -= step + 1;
        }
        else
        {
            count = step;
        }
    }

    return not(first == container.end()) and not(value < *first) ? u32(first - container.begin())
                                                                 : Maybe<u32>();
}

template<Concepts::ContiguousContainer Container>
constexpr void sort(Container& container)
{
    std::sort(container.begin(), container.end());
}

template<Concepts::ContiguousContainer Container, typename Predicate>
constexpr void sort(Container& container, Predicate&& predicate)
{
    std::sort(container.begin(), container.end(), std::forward<Predicate>(predicate));
}

template<Concepts::ContiguousContainer Container, typename U>
constexpr decltype(auto) binaryFind(Container& container, const U& value)
{
    const auto idx = binaryFindIndex(container, value);

    return idx ? &container[*idx] : nullptr;
}

template<Concepts::ContiguousContainer Container, typename U>
constexpr decltype(auto) binaryFind(const Container& container, const U& value)
{
    const auto idx = binaryFindIndex(container, value);

    return idx ? &container[*idx] : nullptr;
}

template<Concepts::ForwardContainer Container, typename U>
requires(std::convertible_to<U, typename Container::value_type>)
constexpr void fill(Container& container, u32 startIndex, u32 count, const U& value)
{
    auto it = container.begin() + startIndex;

    for (u32 i = 0; i < count; ++i, ++it)
    {
        *it = value;
    }
}

template<Concepts::ForwardContainer Container, typename U>
requires(std::convertible_to<U, typename Container::value_type>)
constexpr void fillByIncrementing(Container& container, U initialValue)
{
    for (auto& item : container)
    {
        item = initialValue;
        ++initialValue;
    }
}

template<Concepts::ContiguousContainer T, typename Func>
constexpr void reverseIterate(const T& container, Func&& func)
{
    if (container.isEmpty())
    {
        return;
    }

    const auto* items = container.data();
    const auto  size  = static_cast<ptrdiff_t>(container.size());

    for (auto i = size - 1; i >= 0; --i)
    {
        if (not std::invoke(func, items[i]))
        {
            return;
        }
    }
}

template<Concepts::ContiguousContainer Container>
void shuffle(Container& container)
{
    if (container.isEmpty())
    {
        return;
    }

    auto*      data = container.data();
    const auto size = static_cast<ptrdiff_t>(container.size());

    for (int64_t i = size - 1; i > 0; --i)
    {
        std::swap(data[i], data[Random::nextInt({0, u32(i)})]);
    }
}

template<Concepts::ContiguousContainer Container>
Maybe<const typename Container::value_type&> randomItem(const Container& container)
{
    if (container.isEmpty())
    {
        return none;
    }

    const auto idx = Random::nextInt({0, container.size() - 1});

    return Maybe<const typename Container::value_type&>(container[idx]);
}

template<Concepts::ForwardContainer Container, typename value_type = typename Container::value_type>
requires Concepts::LessComparable<value_type>
constexpr Maybe<const value_type&> maxItem(const Container& container)
{
    if (container.isEmpty())
    {
        return none;
    }

    auto       it       = container.begin();
    const auto end      = container.end();
    auto       maxValue = it;
    ++it;

    while (it != end)
    {
        if (*maxValue < *it)
        {
            maxValue = it;
        }

        ++it;
    }

    return maxValue;
}

template<Concepts::ForwardContainer Container, typename Predicate>
constexpr auto maxItemBy(Container&& container, Predicate&& predicate)
{
    using ItemType = std::invoke_result_t<Predicate, decltype(*container.begin())>;

    auto maxValue = Maybe<ItemType>();

    if (not container.isEmpty())
    {
        auto       it  = container.begin();
        const auto end = container.end();
        maxValue       = std::invoke(predicate, *it);
        ++it;

        while (it != end)
        {
            auto value = std::invoke(predicate, *it);
            if (maxValue < value)
            {
                maxValue = std::move(value);
            }

            ++it;
        }
    }

    return maxValue;
}

template<Concepts::ForwardContainer Container, typename ValueType = typename Container::value_type>
requires Concepts::Summable<ValueType>
constexpr ValueType sum(const Container& container)
{
    auto result = ValueType();

    for (const auto& value : container)
    {
        result += value;
    }

    return result;
}

template<
    Concepts::ForwardContainer Container,
    typename Func,
    typename ValueType  = typename Container::value_type,
    typename ResultType = std::invoke_result_t<Func, ValueType>>
requires Concepts::Summable<ResultType>
constexpr ResultType sumBy(const Container& container, Func&& func)
{
    auto result = ResultType();

    for (const auto& value : container)
    {
        result += std::invoke(func, value);
    }

    return result;
}

/// Removes duplicate elements from a list in a stable manner.
///
/// Element comparison is done via `operator==`.
template<typename T>
requires Concepts::EqualityComparable<T>
void removeDuplicatesButKeepOrder(T& list)
{
    auto i = 0u;

    while (i != list.size())
    {
        auto j = i;
        ++j;

        while (j != list.size())
        {
            if (list[j] == list[i])
            {
                list.removeAtIterator(list.begin() + j);
            }
            else
            {
                ++j;
            }
        }

        ++i;
    }
}

template<Concepts::ForwardContainer Container1, Concepts::ForwardContainer Container2>
requires Concepts::EqualityComparable<typename Container1::value_type, typename Container2::value_type>
constexpr Maybe<u32> mismatch(const Container1& container1, const Container2& container2)
{
    auto it1 = container1.begin();
    auto it2 = container2.begin();

    const auto doMismatch = [](auto primaryIt, auto secondaryIt, auto end) -> Maybe<u32>
    { // NOLINT(*-trailing-return)
        auto idx = u32(0);

        while (primaryIt != end && *primaryIt == *secondaryIt)
        {
            ++primaryIt, ++secondaryIt, ++idx;
        }

        return primaryIt != end ? idx : none;
    };

    return container1.size() < container2.size() ? doMismatch(it1, it2, container1.end())
                                                 : doMismatch(it2, it1, container2.end());
}

template<typename InputPtr, typename OutputPtr>
requires(std::is_pointer_v<InputPtr> and std::is_pointer_v<OutputPtr>)
OutputPtr move(InputPtr first, InputPtr last, OutputPtr dstFirst)
{
    for (; first != last; ++dstFirst, ++first)
    {
        *dstFirst = std::move(*first);
    }

    return dstFirst;
}

template<typename Ptr1, typename Ptr2>
requires(std::is_pointer_v<Ptr1> and std::is_pointer_v<Ptr2>)
Ptr2 moveBackward(Ptr1 first, Ptr1 last, Ptr2 dstLast)
{
    while (first != last)
    {
        *--dstLast = std::move(*--last);
    }

    return dstLast;
}

template<Concepts::ListLike DstContainer, Concepts::ForwardContainer SrcContainer, typename Func>
requires Concepts::HasAddItem<DstContainer, std::invoke_result_t<Func, typename SrcContainer::value_type>>
         and std::invocable<Func, typename SrcContainer::value_type>
         and std::constructible_from<
             typename DstContainer::value_type,
             std::invoke_result_t<Func, typename SrcContainer::value_type>>
[[nodiscard]]
DstContainer mapTo(const SrcContainer& container, Func&& func)
{
    auto result_container = DstContainer();
    result_container.reserve(container.size());

    for (const auto& item : container)
    {
        result_container.add(std::move(std::invoke(func, item)));
    }

    return result_container;
}

template<Concepts::ListLike DstContainer, Concepts::ForwardContainer SrcContainer, typename Func>
[[nodiscard]]
DstContainer filterMapTo(const SrcContainer& container, Func&& func)
{
    auto result_container = DstContainer();
    result_container.reserve(container.size());

    for (const auto& item : container)
    {
        if (auto value = std::invoke(func, item))
        {
            result_container.add(std::move(*value));
        }
    }

    return result_container;
}

int levensteinDistance(StringView s1, StringView s2);

template<Concepts::ForwardContainer Container, typename Predicate>
requires std::invocable<Predicate, typename Container::value_type>
String joinToStringBy(const Container& container, StringView delimiter, Predicate&& predicate);

template<Concepts::ForwardContainer Container>
requires Concepts::HasToString<typename Container::value_type>
String joinToString(const Container& container, StringView delimiter);

template<Concepts::ListLike ResultList>
requires std::is_same_v<typename ResultList::value_type, String>
[[nodiscard]]
ResultList splitString(StringView str, StringView delimiters);
} // namespace Polly

#include "Polly/StringView.hpp"

#include "Polly/ToString.hpp" // NOLINT(*-duplicate-include)

namespace Polly
{
template<Concepts::ForwardContainer Container, typename Predicate>
requires std::invocable<Predicate, typename Container::value_type>
String joinToStringBy(const Container& container, StringView delimiter, Predicate&& predicate)
{
    auto str = String();

    for (const auto& value : container)
    {
        str += std::invoke(predicate, value);
        str += delimiter;
    }

    if (not str.isEmpty())
    {
        str.removeLast(delimiter.size());
    }

    return str;
}

template<Concepts::ForwardContainer Container>
requires Concepts::HasToString<typename Container::value_type>
String joinToString(const Container& container, StringView delimiter)
{
    return joinToStringBy(
        container,
        delimiter,
        [](const typename Container::value_type& value) { return toString(value); });
}

template<Concepts::ListLike ResultList>
requires std::is_same_v<typename ResultList::value_type, String>
[[nodiscard]]
ResultList splitString(StringView str, StringView delimiters)
{
    auto result = ResultList();

    auto start = u32(0);
    auto end   = str.findAnyOf(delimiters);

    while (end)
    {
        if (const auto s = str.substring(start, *end - start); !s.isEmpty())
        {
            result.emplace(s);
        }

        start = *end + 1;

        if (start < str.size())
        {
            end = str.findAnyOf(delimiters, start);
        }
        else
        {
            break;
        }
    }

    if (const auto s = str.substring(start); not s.isEmpty())
    {
        result.emplace(s);
    }

    return result;
}
} // namespace Polly
