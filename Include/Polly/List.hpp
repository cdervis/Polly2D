// Copyright (C) 2025 Cem Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE.

// This file contains portions of code from the small_vector library, modified for use with Polly:

/**
 *
 * Copyright © 2020-2021 Gene Harvey
 *
 * This software may be modified and distributed under the terms
 * of the MIT license. See the LICENSE file for details.
 */

#pragma once

#include "Polly/Assume.hpp"
#include "Polly/Concepts.hpp"
#include "Polly/Details/ListBase.hpp"
#include "Polly/Maybe.hpp"
#include "Polly/Prerequisites.hpp"
#include "Polly/Span.hpp"
#include <functional>
#include <initializer_list>
#include <limits>

namespace Polly
{
class ByteBlob;

namespace Details::concepts
{
template<typename T>
concept Complete = requires { sizeof(T); };

template<typename T>
concept TriviallyDestructible = std::is_trivially_destructible_v<T>;

template<typename T>
concept NoThrowDestructible = std::is_nothrow_destructible_v<T>;

// Note: this mirrors the named requirements, not the standard library concepts,
// so we don't require Destructible here.

template<typename T, typename... Args>
concept ConstructibleFrom = std::is_constructible_v<T, Args...>;

template<typename From, typename To>
concept ConvertibleTo = std::is_convertible_v<From, To>
                        and requires(std::add_rvalue_reference_t<From> (&f)()) { static_cast<To>(f()); };

template<typename T>
concept MoveAssignable = std::assignable_from<T&, T>;

template<typename T>
concept CopyAssignable = MoveAssignable<T>
                         and std::assignable_from<T&, T&>
                         and std::assignable_from<T&, const T&>
                         and std::assignable_from<T&, const T>;

template<typename T>
concept MoveConstructible = ConstructibleFrom<T, T> and ConvertibleTo<T, T>;

template<typename T>
concept Swappable = std::swappable<T>;

template<typename T>
concept EqualityComparable = std::equality_comparable<T>;

template<typename T, typename... Args>
concept EmplaceConstructible = requires(T* p, Args&&... args) {
    { std::construct_at(p, std::forward<Args>(args)...) } -> std::same_as<T*>;
};

template<typename T>
concept DefaultInsertable = EmplaceConstructible<T>;

template<typename T>
concept MoveInsertable = EmplaceConstructible<T, T>;

template<typename T>
concept CopyInsertable =
    MoveInsertable<T> and EmplaceConstructible<T, T&> and EmplaceConstructible<T, const T&>;

// same method as with EmplaceConstructible
template<typename T>
concept Erasable = std::is_destructible_v<T>;

template<typename T>
concept BoolConstant = std::derived_from<T, std::true_type> or std::derived_from<T, std::false_type>;

namespace cer
{
// Basically, these shut off the concepts if we have an incomplete type.
// This namespace is only needed because of issues on Clang
// preventing us from short-circuiting for incomplete types.

template<typename T>
concept MoveAssignable = not concepts::Complete<T> or concepts::MoveAssignable<T>;

template<typename T>
concept CopyAssignable = not concepts::Complete<T> or concepts::CopyAssignable<T>;

template<typename T>
concept MoveConstructible = not concepts::Complete<T> or concepts::MoveConstructible<T>;

template<typename T>
concept Swappable = not concepts::Complete<T> or concepts::Swappable<T>;

template<typename T>
concept DefaultInsertable = not concepts::Complete<T> or concepts::DefaultInsertable<T>;

template<typename T>
concept MoveInsertable = not concepts::Complete<T> or concepts::MoveInsertable<T>;

template<typename T>
concept CopyInsertable = not concepts::Complete<T> or concepts::CopyInsertable<T>;

template<typename T>
concept Erasable = not concepts::Complete<T> or concepts::Erasable<T>;

template<typename T, typename... Args>
concept EmplaceConstructible = not concepts::Complete<T> or concepts::EmplaceConstructible<T, Args...>;
} // namespace cer
} // namespace Details::concepts


template<typename T, u32 InlineCapacity = 0>
class List : Details::ListBase<T, InlineCapacity>
{
    friend ByteBlob;

    using base = Details::ListBase<T, InlineCapacity>;

  public:
    using value_type                            = T;
    static constexpr auto isContiguousContainer = true;
    static constexpr auto isForwardContainer    = true;
    using iterator                              = T*;
    using const_iterator                        = const T*;

    static_assert(
        InlineCapacity <= (std::numeric_limits<u32>::max)(),
        "InlineCapacity must be less than or equal to the maximum value of u32.");

    static constexpr u32 inlineCapacityV = InlineCapacity;

  private:
    static constexpr bool moveAssignable    = Details::concepts::MoveAssignable<T>;
    static constexpr bool copyAssignable    = Details::concepts::CopyAssignable<T>;
    static constexpr bool moveConstructible = Details::concepts::MoveConstructible<T>;
    static constexpr bool swappable         = Details::concepts::Swappable<T>;
    static constexpr bool defaultInsertable = Details::concepts::DefaultInsertable<T>;
    static constexpr bool moveInsertable    = Details::concepts::MoveInsertable<T>;
    static constexpr bool copyInsertable    = Details::concepts::CopyInsertable<T>;
    static constexpr bool erasable          = Details::concepts::Erasable<T>;

    template<typename... Args>
    struct EmplaceConstructible
    {
        static constexpr bool value = Details::concepts::EmplaceConstructible<T, Args...>;
    };

  public:
    List() = default;

    List(const List& other)
    requires(copyInsertable)
        : base(base::bypass, other)
    {
    }

    List(List&& other) noexcept
    requires(moveInsertable)
        : base(base::bypass, std::move(other))
    {
    }

    explicit List(u32 count)
    requires(defaultInsertable)
        : base(count)
    {
    }

    List(u32 count, const T& value)
    requires(copyInsertable)
        : base(count, value)
    {
    }

    List(std::initializer_list<T> init)
        : List()
    {
        reserve(static_cast<u32>(init.size()));
        base::appendRange(std::move(init));
    }

    explicit List(Span<T> span)
        : List()
    {
        reserve(span.size());
        base::appendRange(span);
    }

    template<u32 I>
    requires(copyInsertable)
    explicit List(const List<T, I>& other)
        : base(base::bypass, other)
    {
    }

    template<u32 I>
    requires(moveInsertable)
    explicit List(List<T, I>&& other)
        : base(base::bypass, std::move(other))
    {
    }

    ~List()
    requires(erasable)
    = default;

    List& operator=(const List& other)
    requires(copyInsertable and copyAssignable)
    {
        assign(other);
        return *this;
    }

    List& operator=(List&& other) noexcept
    requires(moveInsertable and moveAssignable)
    {
        assign(std::move(other));
        return *this;
    }

    List& operator=(std::initializer_list<T> initList)
    requires(copyInsertable and copyAssignable)
    {
        base::assignWithRange(initList);
        return *this;
    }

    void assign(u32 count, const T& value)
    requires(copyInsertable and copyAssignable)
    {
        base::assignWithCopies(count, value);
    }

    void assign(std::initializer_list<T> list)
    requires(EmplaceConstructible<const T&>::value)
    {
        assign(list.begin(), list.end());
    }

    void assign(const List& other)
    requires(copyInsertable and copyAssignable)
    {
        if (&other != this)
        {
            base::copyAssign(other);
        }
    }

    template<u32 I>
    requires(copyInsertable and copyAssignable)
    void assign(const List<T, I>& other)
    {
        base::copyAssign(other);
    }

    void assign(List&& other) // NOLINT(*-rvalue-reference-param-not-moved)
    requires(moveInsertable and moveAssignable)
    {
        if (&other != this)
        {
            base::moveAssign(other);
        }
    }

    void swap(List& other) noexcept
    requires(moveInsertable and moveAssignable and swappable) or (InlineCapacity == 0)
    {
        base::swap(other);
    }

    iterator begin()
    {
        return iterator(base::beginPtr());
    }

    const_iterator begin() const
    {
        return const_iterator(base::beginPtr());
    }

    iterator end()
    {
        return iterator(base::endPtr());
    }

    const_iterator end() const
    {
        return const_iterator(base::endPtr());
    }

    T& operator[](u32 pos)
    {
        checkIndex(pos);
        return begin()[static_cast<ptrdiff_t>(pos)];
    }

    const T& operator[](u32 pos) const
    {
        checkIndex(pos);
        return begin()[static_cast<ptrdiff_t>(pos)];
    }

    T& first()
    {
        checkEmptyAccess();
        return (*this)[0];
    }

    const T& first() const
    {
        checkEmptyAccess();
        return (*this)[0];
    }

    T& last()
    {
        checkEmptyAccess();
        return (*this)[size() - 1];
    }

    const T& last() const
    {
        checkEmptyAccess();
        return (*this)[size() - 1];
    }

    T* data()
    {
        checkEmptyAccess();
        return base::beginPtr();
    }

    const T* data() const
    {
        checkEmptyAccess();
        return base::beginPtr();
    }

    u32 size() const
    {
        return static_cast<u32>(base::size());
    }

    [[nodiscard]]
    bool isEmpty() const
    {
        return size() == 0;
    }

    u32 capacity() const
    {
        return static_cast<u32>(base::capacity());
    }

    iterator removeFirst()
    requires(moveAssignable and erasable)
    {
        return removeAtIterator(begin());
    }

    template<typename Predicate>
    iterator removeFirstWhere(const Predicate& predicate)
    requires(moveAssignable and erasable)
    {
        auto       it = begin();
        const auto e  = end();

        while (it != e)
        {
            if (std::invoke(predicate, *it))
            {
                return removeAtIterator(it);
            }

            ++it;
        }

        return e;
    }

    template<typename Predicate>
    u32 removeAllWhere(const Predicate& predicate)
    requires(moveAssignable and erasable)
    {
        if (isEmpty())
        {
            return 0;
        }

        auto  removedCount = 0u;
        auto* items        = data();

        for (auto i = static_cast<i64>(size()) - 1; i >= 0; --i)
        {
            if (std::invoke(predicate, items[i]))
            {
                removeAt(static_cast<u32>(i));
                ++removedCount;
            }
        }

        return removedCount;
    }

    template<typename U>
    requires(Concepts::EqualityComparable<T, U>)
    bool removeFirst(const U& value)
    {
        if (const auto idx = indexOf(*this, value))
        {
            removeAt(*idx);
            return true;
        }

        return false;
    }

    template<typename U>
    requires(Concepts::EqualityComparable<T, U>)
    bool removeAll(const U& value)
    {
        if (isEmpty())
        {
            return false;
        }

        auto haveRemovedAny = false;

        const auto* const items = data();

        for (auto i = static_cast<int64_t>(size()) - 1; i >= 0; --i)
        {
            if (items[i] == value)
            {
                removeAt(i);
                haveRemovedAny |= true;
            }
        }

        return haveRemovedAny;
    }

    void removeAt(u32 index)
    requires(moveAssignable and erasable)
    {
        removeAtIterator(begin() + index);
    }

    iterator removeAtIterator(const_iterator it)
    requires(moveAssignable and erasable)
    {
        assumeWithMsg(0 <= it - begin(), "`pos` is out of bounds (before `begin ()`).");
        assumeWithMsg(0 < end() - it, "`pos` is out of bounds (at or after `end ()`).");

        return iterator(base::eraseAt(base::ptrCast(it)));
    }

    void removeAtUnordered(u32 index)
    requires(moveAssignable and erasable)
    {
        if (size() > 1)
        {
            swap(*this[index], last());
            removeLast();
        }
        else
        {
            removeAtIterator(begin() + index);
        }
    }

    iterator removeRange(const_iterator first, const_iterator last)
    requires(moveAssignable and erasable)
    {
        assumeWithMsg(0 <= last - first, "Invalid range.");
        assumeWithMsg(0 <= first - begin(), "`first` is out of bounds (before `begin ()`).");
        assumeWithMsg(0 <= end() - last, "`last` is out of bounds (after `end ()`).");

        return iterator(base::eraseRange(base::ptrCast(first), base::ptrCast(last)));
    }

    T& add(const T& value)
    requires(copyInsertable)
    {
        return emplace(value);
    }

    T& add(T&& value)
    requires(moveInsertable)
    {
        return emplace(std::move(value));
    }

    template<typename... Args>
    requires(EmplaceConstructible<Args...>::value and moveInsertable)
    T& emplace(Args&&... args)
    {
        return *base::appendElement(std::forward<Args>(args)...);
    }

    T& addAt(u32 idx, const T& value)
    requires(copyInsertable)
    {
        checkIndexOut(idx);
        return *base::emplaceAt(begin() + idx, value);
    }

    T& addAt(u32 idx, T&& value)
    requires(moveInsertable)
    {
        checkIndexOut(idx);
        return *base::emplaceAt(begin() + idx, std::move(value));
    }

    iterator addAtIterator(iterator it, const T& value)
    requires(copyInsertable)
    {
        return base::emplaceAt(it, value);
    }

    iterator addAtIterator(iterator it, T&& value)
    requires(moveInsertable)
    {
        return base::emplaceAt(it, std::move(value));
    }

    template<typename... Args>
    requires(EmplaceConstructible<Args...>::value and moveInsertable)
    T& emplaceAt(u32 idx, Args&&... args)
    {
        checkIndexOut(idx);
        return *base::emplaceAt(begin() + idx, std::forward<Args>(args)...);
    }

    template<typename Container>
    void addRange(const Container& container)
    {
        if constexpr (requires { container.size(); })
        {
            reserve(static_cast<u32>(container.size()));
        }

        for (const auto& value : container)
        {
            emplace(value);
        }
    }

    template<typename Container>
    void addRangeAt(u32 index, const Container& container)
    {
        base::insertRange(base::beginPtr() + index, container);
    }

    void removeLast()
    requires(erasable)
    {
        assume(not isEmpty() and "`pop_back ()` called on an empty List.");
        base::eraseLast();
    }

    void moveItemAt(u32 fromIdx, u32 toIdx)
    {
        checkIndexOut(fromIdx);
        checkIndexOut(toIdx);

        if (fromIdx == toIdx)
        {
            return;
        }

        auto it    = begin() + fromIdx;
        auto value = std::move(*it);
        removeAtIterator(it);
        base::emplaceAt(begin() + toIdx, std::move(value));
    }

    iterator moveItemAtIterator(iterator from, iterator to)
    {
        if (from == to)
        {
            return from;
        }

        auto value = std::move(*from);
        removeAtIterator(from);
        return base::emplaceAt(to, std::move(value));
    }

    void reserve(u32 newCapacity)
    requires(moveInsertable)
    {
        base::requestCapacity(newCapacity);
    }

    void shrinkToFit()
    requires(moveInsertable)
    {
        base::shrinkToSize();
    }

    void clear()
    requires(erasable)
    {
        base::eraseAll();
    }

    void resize(u32 count)
    requires(moveInsertable and defaultInsertable)
    {
        base::resizeWith(count);
    }

    void resize(u32 count, const T& value)
    requires(copyInsertable)
    {
        base::resizeWith(count, value);
    }

    [[nodiscard]]
    bool isSmall() const
    {
        return not base::hasAllocation();
    }

    [[nodiscard]]
    static consteval u32 smallCapacity()
    {
        return inlineCapacityV;
    }

    template<
        u32 NewInlineCapacity = InlineCapacity,
        typename Func,
        typename Result = std::invoke_result_t<Func, T>>
    List<Result, NewInlineCapacity> map(const Func& func) const
    {
        auto result = List<Result, NewInlineCapacity>();

        for (const auto& item : *this)
        {
            result.add(func(item));
        }

        return result;
    }

    // ReSharper disable once CppNonExplicitConversionOperator
    operator Span<T>() const
    {
        return isEmpty() ? Span<T>() : Span(data(), size());
    }

    // ReSharper disable once CppNonExplicitConversionOperator
    operator MutableSpan<T>()
    {
        return isEmpty() ? MutableSpan<T>() : MutableSpan(data(), size());
    }

  private:
    void releaseData()
    {
        base::releaseData();
    }

    void checkIndex([[maybe_unused]] u32 idx) const
    {
#ifndef polly_no_hardening
        if (idx >= size())
        {
            throw Error("Attempting to access a List out of bounds.");
        }
#endif
    }

    void checkIndexOut([[maybe_unused]] u32 idx) const
    {
#ifndef polly_no_hardening
        if (idx > size())
        {
            throw Error("Attempting to access a List out of bounds.");
        }
#endif
    }

    void checkEmptyAccess() const
    {
#ifndef polly_no_hardening
        if (isEmpty())
        {
            throw Error("Attempting to access an empty List.");
        }
#endif
    }
};

template<typename T, u32 InlineCapacityLHS, u32 InlineCapacityRHS>
bool operator==(const List<T, InlineCapacityLHS>& lhs, const List<T, InlineCapacityRHS>& rhs)
{
    return lhs.size() == rhs.size() and std::equal(lhs.begin(), lhs.end(), rhs.begin());
}

template<typename T, u32 InlineCapacity>
bool operator==(const List<T, InlineCapacity>& lhs, const List<T, InlineCapacity>& rhs)
{
    return lhs.size() == rhs.size() and std::equal(lhs.begin(), lhs.end(), rhs.begin());
}

template<typename T, u32 InlineCapacityLHS, u32 InlineCapacityRHS>
requires(std::three_way_comparable<T>)
constexpr auto operator<=>(const List<T, InlineCapacityLHS>& lhs, const List<T, InlineCapacityRHS>& rhs)
{
    return std::lexicographical_compare_three_way(
        lhs.begin(),
        lhs.end(),
        rhs.begin(),
        rhs.end(),
        std::compare_three_way{});
}

template<typename T, u32 InlineCapacity>
requires(std::three_way_comparable<T>)
constexpr auto operator<=>(const List<T, InlineCapacity>& lhs, const List<T, InlineCapacity>& rhs)
{
    return std::lexicographical_compare_three_way(
        lhs.begin(),
        lhs.end(),
        rhs.begin(),
        rhs.end(),
        std::compare_three_way{});
}

template<typename T, u32 InlineCapacityLHS, u32 InlineCapacityRHS>
constexpr auto operator<=>(const List<T, InlineCapacityLHS>& lhs, const List<T, InlineCapacityRHS>& rhs)
{
    constexpr auto comparison = [](const T& l, const T& r)
    {
        return l < r   ? std::weak_ordering::less
               : r < l ? std::weak_ordering::greater
                       : std::weak_ordering::equivalent;
    };

    return std::lexicographical_compare_three_way(lhs.begin(), lhs.end(), rhs.begin(), rhs.end(), comparison);
}

template<typename T, u32 InlineCapacity>
constexpr auto operator<=>(const List<T, InlineCapacity>& lhs, const List<T, InlineCapacity>& rhs)
{
    constexpr auto comparison = [](const T& l, const T& r)
    {
        return l < r   ? std::weak_ordering::less
               : r < l ? std::weak_ordering::greater
                       : std::weak_ordering::equivalent;
    };

    return std::lexicographical_compare_three_way(lhs.begin(), lhs.end(), rhs.begin(), rhs.end(), comparison);
}


template<typename T>
using SmallList = List<T, 3>;
} // namespace Polly
