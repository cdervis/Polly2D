// Copyright (C) 2025 Cemalettin Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE.

#pragma once

#include "Polly/Comparers.hpp"
#include "Polly/Prerequisites.hpp"
#include <concepts>
#include <utility>

namespace Polly
{
class String;
class StringView;
class Any;

namespace Concepts
{
/// Defines the concept of a number, either integral or floating-point.
template<typename T>
concept Number = std::is_arithmetic_v<T> and not std::is_same_v<bool, T>;

template<typename T, typename Comparer>
concept Comparable = requires(T lhs, T rhs, Comparer c) {
    { c(lhs, rhs) } -> std::convertible_to<bool>;
};

template<typename T, typename U>
concept HasCompoundAdd = requires(T lhs, U rhs) {
    { lhs += rhs };
};

template<typename T, typename U = T>
concept Summable = std::is_default_constructible_v<T> and HasCompoundAdd<T, U>;

template<typename T, typename Comparer>
concept IsDefaultLess = std::is_same_v<Comparer, Comparers::Less<T>>;

template<typename T, typename Comparer>
concept IsDefaultEqual = std::is_same_v<Comparer, Comparers::Equal<T>>;

template<typename T, typename Comparer = Comparers::Less<T>>
concept LessComparable = (IsDefaultLess<T, Comparer> and requires(T lhs, T rhs) { lhs < rhs; })
                         or (not IsDefaultLess<T, Comparer> and Comparable<T, Comparer>);

template<typename T, typename U = T, typename Comparer = Comparers::Equal<T>>
concept EqualityComparable = (not std::is_same_v<T, U> and requires(T lhs, U rhs) { lhs == rhs; })
                             or (IsDefaultEqual<T, Comparer> and requires(T lhs, T rhs) { lhs == rhs; })
                             or (not IsDefaultEqual<T, Comparer> and Comparable<T, Comparer>);

template<typename T>
concept ContiguousContainer = requires(std::remove_cvref_t<T> t) {
    { std::remove_cvref_t<T>::isContiguousContainer } -> std::convertible_to<bool>;
    { t.begin() };
    { t.end() };
    { t.data() };
    { t.size() } -> std::convertible_to<u32>;
    { t.isEmpty() } -> std::convertible_to<bool>;
} and T::isContiguousContainer;

template<typename T>
concept ForwardContainer = ContiguousContainer<T> or requires(std::remove_cvref_t<T> t) {
    { std::remove_cvref_t<T>::isForwardContainer } -> std::convertible_to<bool>;
    { t.begin() };
    { t.end() };
    { t.size() } -> std::convertible_to<u32>;
    { t.isEmpty() } -> std::convertible_to<bool>;
};

template<typename Container>
concept ListLike = ContiguousContainer<Container>
                   and std::is_default_constructible_v<Container>
                   and std::is_move_constructible_v<Container>
                   and requires(Container c) {
                           { c.reserve(u32()) };
                           { c.clear() };
                       };

template<typename Container, typename Item>
concept HasAddItem = requires(Container c, Item item) {
    { c.add(std::forward<Item>(item)) };
};

template<typename T>
concept StringLiteral =
    std::is_same_v<char*, std::decay_t<T>> or std::is_same_v<const char*, std::decay_t<T>>;

template<typename From, typename To>
concept CanConstructStringView =
    std::is_constructible_v<StringView, From> or (StringLiteral<From> and std::is_same_v<To, Any>);

template<typename T>
concept StringOrStringView = std::is_same_v<T, String> or std::is_same_v<T, StringView>;

template<typename T>
concept SpanCompatible = not std::is_const_v<T>;

template<typename T>
concept MutableSpanCompatible = not std::is_const_v<T>;
} // namespace concepts
} // namespace Polly
