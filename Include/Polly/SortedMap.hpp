// Copyright (C) 2025 Cem Dervis
// This file is part of Polly, a 2D C++ game framework for minimalists.
// For conditions of distribution and use, see copyright notice in LICENSE, or https://polly2d.org.

#pragma once

#include "Polly/Concepts.hpp"
#include "Polly/Details/BinaryTree.hpp"
#include "Polly/Pair.hpp"
#include "Polly/String.hpp"
#include "Polly/StringView.hpp"
#include <initializer_list>

namespace Polly
{
template<typename Key, typename Value, typename Comparer = Comparers::Less<Key>>
requires(Concepts::LessComparable<Key, Comparer>)
class SortedMap
{
  public:
    using value_type                         = Pair<Key, Value>;
    static constexpr auto isForwardContainer = true;

    SortedMap() = default;

    SortedMap(const std::initializer_list<value_type>& items);

    template<typename UKey, typename UValue>
    Maybe<value_type&> add(Pair<UKey, UValue>&& pair);

    template<typename UKey, typename UValue>
    Maybe<value_type&> add(UKey&& key, UValue&& value);

    template<typename Container>
    void addRange(Container&& container);

    bool contains(const Key& key) const;

    Maybe<Value&> find(const Key& key);

    Maybe<const Value&> find(const Key& key) const;

    // Built-in support for StringView-based lookups when the key is String.

    template<typename U>
    Maybe<Value&> find(U key)
    requires(std::is_same_v<Key, String> and std::convertible_to<U, StringView>);

    template<typename U>
    Maybe<const Value&> find(U key) const
    requires(std::is_same_v<Key, String> and std::convertible_to<U, StringView>);

    template<typename Predicate>
    Maybe<const Value&> findWhere(Predicate&& predicate) const;

    void clear();

    bool remove(const Key& value);

    template<typename U>
    requires(std::is_same_v<Key, String> and std::convertible_to<U, StringView>)
    bool remove(U key);

    template<typename Container>
    u32 removeRange(const Container& container);

    template<typename Predicate>
    u32 removeWhere(Predicate&& predicate);

    template<Concepts::ListLike DstContainer>
    requires(
        Concepts::HasAddItem<DstContainer, Key>
        and std::constructible_from<typename DstContainer::value_type, Key>)
    [[nodiscard]]
    DstContainer keysTo() const
    {
        auto result = DstContainer();
        result.reserve(size());

        for (const auto& [key, _] : *this)
        {
            result.add(key);
        }

        return result;
    }

    template<Concepts::ListLike DstContainer>
    requires(
        Concepts::HasAddItem<DstContainer, Value>
        and std::constructible_from<typename DstContainer::value_type, Value>)
    [[nodiscard]]
    DstContainer valuesTo() const
    {
        auto result = DstContainer();
        result.reserve(size());

        for (const auto& [_, value] : *this)
        {
            result.add(value);
        }

        return result;
    }

    bool isEmpty() const;

    u32 size() const;

    auto begin();

    auto begin() const;

    auto end();

    auto end() const;

    Value& operator[](const Key& key) PollyLifetimeBound;

    const Value& operator[](const Key& key) const PollyLifetimeBound;

  private:
    struct PairComparer final
    {
        // Compare two exact pairs
        constexpr bool operator()(const value_type& lhs, const value_type& rhs) const;

        // Compare two arbitrary pairs
        template<typename K1, typename V1, typename K2, typename V2>
        constexpr bool operator()(const Pair<K1, V1>& lhs, const Pair<K2, V2>& rhs) const;

        // Compare arbitrary value with arbitrary pair
        template<typename K, typename V, typename U>
        constexpr bool operator()(const U& lhs, const Pair<K, V>& rhs) const;

        // Compare arbitrary pair with arbitrary value
        template<typename K, typename V, typename U>
        constexpr bool operator()(const Pair<K, V>& lhs, const U& rhs) const;
    };

    Details::BinaryTree<value_type, PairComparer> _tree;
};

template<typename Key, typename Value, typename Comparer1, typename Comparer2>
static bool operator==(
    const SortedMap<Key, Value, Comparer1>& lhs,
    const SortedMap<Key, Value, Comparer2>& rhs);

template<typename Key, typename Value, typename Comparer1, typename Comparer2>
static bool operator!=(
    const SortedMap<Key, Value, Comparer1>& lhs,
    const SortedMap<Key, Value, Comparer2>& rhs);
} // namespace Polly

#include "Polly/Details/SortedMap.inl"