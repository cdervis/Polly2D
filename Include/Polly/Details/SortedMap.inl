// Copyright (C) 2025 Cem Dervis
// This file is part of Polly, a minimalistic 2D C++ game framework.
// For conditions of distribution and use, see copyright notice in LICENSE, or https://polly2d.org.

#pragma once

#include "Polly/ToString.hpp"

namespace Polly
{
namespace Details
{
void throwKeyNotFoundException(StringView key);

template<typename ValueType>
struct SortedMapStringComparer
{
    bool operator()(StringView lhs, const ValueType& rhs) const
    {
        return lhs < rhs.first;
    }

    bool operator()(const ValueType& lhs, StringView rhs) const
    {
        return lhs.first < rhs;
    }
};
} // namespace Details

template<typename Key, typename Value, typename Comparer>
requires(Concepts::LessComparable<Key, Comparer>)
SortedMap<Key, Value, Comparer>::SortedMap(const std::initializer_list<value_type>& items)
    : SortedMap()
{
    addRange(items);
}

template<typename Key, typename Value, typename Comparer>
requires(Concepts::LessComparable<Key, Comparer>)
template<typename UKey, typename UValue>
Maybe<typename SortedMap<Key, Value, Comparer>::value_type&> SortedMap<Key, Value, Comparer>::add(
    Pair<UKey, UValue>&& pair) // NOLINT(*-rvalue-reference-param-not-moved)
{
    auto result = _tree.add(std::forward<decltype(pair)&&>(pair));
    return result ? result->value : Maybe<value_type&>(none);
}

template<typename Key, typename Value, typename Comparer>
requires(Concepts::LessComparable<Key, Comparer>)
template<typename UKey, typename UValue>
Maybe<typename SortedMap<Key, Value, Comparer>::value_type&> SortedMap<Key, Value, Comparer>::add(
    UKey&&   key,
    UValue&& value)
{
    return add(Pair(std::forward<UKey>(key), std::forward<UValue>(value)));
}

template<typename Key, typename Value, typename Comparer>
requires(Concepts::LessComparable<Key, Comparer>)
template<typename Container>
void SortedMap<Key, Value, Comparer>::addRange(Container&& container)
{
    _tree.addRange(std::forward<Container>(container));
}

template<typename Key, typename Value, typename Comparer>
requires(Concepts::LessComparable<Key, Comparer>)
bool SortedMap<Key, Value, Comparer>::contains(const Key& key) const
{
    return _tree.findNode(key) != nullptr;
}

template<typename Key, typename Value, typename Comparer>
requires(Concepts::LessComparable<Key, Comparer>)
Maybe<Value&> SortedMap<Key, Value, Comparer>::find(const Key& key)
{
    auto* node = _tree.findNode(key);
    return node ? Maybe<Value&>(node->value.second) : Maybe<Value&>();
}

template<typename Key, typename Value, typename Comparer>
requires(Concepts::LessComparable<Key, Comparer>)
Maybe<const Value&> SortedMap<Key, Value, Comparer>::find(const Key& key) const
{
    const auto* node = _tree.findNode(key);
    return node ? Maybe<const Value&>(node->value.second) : Maybe<const Value&>();
}

template<typename Key, typename Value, typename Comparer>
requires(Concepts::LessComparable<Key, Comparer>)
template<typename U>
Maybe<Value&> SortedMap<Key, Value, Comparer>::find(U key)
requires(std::is_same_v<Key, String> and std::convertible_to<U, StringView>)
{
    const auto str_view = StringView(key);
    auto*      node     = _tree.template findNode<Details::SortedMapStringComparer<value_type>>(str_view);

    return node ? Maybe<Value&>(node->value.second) : Maybe<Value&>();
}

template<typename Key, typename Value, typename Comparer>
requires(Concepts::LessComparable<Key, Comparer>)
template<typename U>
Maybe<const Value&> SortedMap<Key, Value, Comparer>::find(U key) const
requires(std::is_same_v<Key, String> and std::convertible_to<U, StringView>)
{
    const auto  str_view = StringView(key);
    const auto* node     = _tree.template findNode<Details::SortedMapStringComparer<value_type>>(str_view);

    return node ? Maybe<const Value&>(node->value.second) : Maybe<const Value&>();
}

template<typename Key, typename Value, typename Comparer>
requires(Concepts::LessComparable<Key, Comparer>)
template<typename Predicate>
Maybe<const Value&> SortedMap<Key, Value, Comparer>::findWhere(Predicate&& predicate) const
{
    if (const auto* node = _tree.findNodeWhere(std::forward<Predicate>(predicate)))
    {
        return Maybe<const Value&>(node->value.second);
    }

    return none;
}

template<typename Key, typename Value, typename Comparer>
requires(Concepts::LessComparable<Key, Comparer>)
void SortedMap<Key, Value, Comparer>::clear()
{
    _tree.clear();
}

template<typename Key, typename Value, typename Comparer>
requires(Concepts::LessComparable<Key, Comparer>)
bool SortedMap<Key, Value, Comparer>::remove(const Key& value)
{
    return _tree.remove(value);
}

template<typename Key, typename Value, typename Comparer>
requires(Concepts::LessComparable<Key, Comparer>)
template<typename U>
requires(std::is_same_v<Key, String> and std::convertible_to<U, StringView>)
bool SortedMap<Key, Value, Comparer>::remove(U key)
{
    const auto str_view = StringView(key);

    if (auto* node = _tree.template findNode<Details::SortedMapStringComparer<value_type>>(str_view))
    {
        _tree.destroyNode(node);
        return true;
    }

    return false;
}

template<typename Key, typename Value, typename Comparer>
requires(Concepts::LessComparable<Key, Comparer>)
template<typename Container>
u32 SortedMap<Key, Value, Comparer>::removeRange(const Container& container)
{
    return _tree.removeRange(container);
}

template<typename Key, typename Value, typename Comparer>
requires(Concepts::LessComparable<Key, Comparer>)
template<typename Predicate>
u32 SortedMap<Key, Value, Comparer>::removeWhere(Predicate&& predicate)
{
    return _tree.removeWhere(std::forward<Predicate>(predicate));
}

template<typename Key, typename Value, typename Comparer>
requires(Concepts::LessComparable<Key, Comparer>)
bool SortedMap<Key, Value, Comparer>::isEmpty() const
{
    return size() == 0;
}

template<typename Key, typename Value, typename Comparer>
requires(Concepts::LessComparable<Key, Comparer>)
u32 SortedMap<Key, Value, Comparer>::size() const
{
    return _tree.size();
}

template<typename Key, typename Value, typename Comparer>
requires(Concepts::LessComparable<Key, Comparer>)
auto SortedMap<Key, Value, Comparer>::begin()
{
    return _tree.begin();
}

template<typename Key, typename Value, typename Comparer>
requires(Concepts::LessComparable<Key, Comparer>)
auto SortedMap<Key, Value, Comparer>::begin() const
{
    return _tree.begin();
}

template<typename Key, typename Value, typename Comparer>
requires(Concepts::LessComparable<Key, Comparer>)
auto SortedMap<Key, Value, Comparer>::end()
{
    return _tree.end();
}

template<typename Key, typename Value, typename Comparer>
requires(Concepts::LessComparable<Key, Comparer>)
auto SortedMap<Key, Value, Comparer>::end() const
{
    return _tree.end();
}

template<typename Key, typename Value, typename Comparer>
requires(Concepts::LessComparable<Key, Comparer>)
Value& SortedMap<Key, Value, Comparer>::operator[](const Key& key)
{
    auto* node = _tree.findNode(key);

#ifndef polly_no_hardening
    if (not node)
    {
        if constexpr (Concepts::HasToString<std::remove_cvref_t<Key>>)
        {
            Details::throwKeyNotFoundException(toString(key));
        }
        else
        {
            Details::throwKeyNotFoundException({});
        }
    }
#endif

    return node->value.second;
}

template<typename Key, typename Value, typename Comparer>
requires(Concepts::LessComparable<Key, Comparer>)
const Value& SortedMap<Key, Value, Comparer>::operator[](const Key& key) const
{
    const auto* node = _tree.findNode(key);

#ifndef polly_no_hardening
    if (not node)
    {
        if constexpr (Concepts::HasToString<std::remove_cvref_t<Key>>)
        {
            Details::throwKeyNotFoundException(toString(key));
        }
        else
        {
            Details::throwKeyNotFoundException({});
        }
    }
#endif

    return node->value.second;
}

template<typename Key, typename Value, typename Comparer>
requires(Concepts::LessComparable<Key, Comparer>)
constexpr bool SortedMap<Key, Value, Comparer>::PairComparer::operator()(
    const value_type& lhs,
    const value_type& rhs) const
{
    return Comparer()(lhs.first, rhs.first);
}

template<typename Key, typename Value, typename Comparer>
requires(Concepts::LessComparable<Key, Comparer>)
template<typename K1, typename V1, typename K2, typename V2>
constexpr bool SortedMap<Key, Value, Comparer>::PairComparer::operator()(
    const Pair<K1, V1>& lhs,
    const Pair<K2, V2>& rhs) const
{
    return Comparer()(lhs.first, rhs.first);
}

template<typename Key, typename Value, typename Comparer>
requires(Concepts::LessComparable<Key, Comparer>)
template<typename K, typename V, typename U>
constexpr bool SortedMap<Key, Value, Comparer>::PairComparer::operator()(const U& lhs, const Pair<K, V>& rhs)
    const
{
    return Comparer()(lhs, rhs.first);
}

template<typename Key, typename Value, typename Comparer>
requires(Concepts::LessComparable<Key, Comparer>)
template<typename K, typename V, typename U>
constexpr bool SortedMap<Key, Value, Comparer>::PairComparer::operator()(const Pair<K, V>& lhs, const U& rhs)
    const
{
    return Comparer()(lhs.first, rhs);
}

template<typename Key, typename Value, typename Comparer1, typename Comparer2>
bool operator==(const SortedMap<Key, Value, Comparer1>& lhs, const SortedMap<Key, Value, Comparer2>& rhs)
{
    if (lhs.size() != rhs.size())
    {
        return false;
    }

    auto       lhs_it = lhs.begin();
    auto       rhs_it = rhs.begin();
    const auto end    = lhs.end();

    while (lhs_it != end)
    {
        if (not(*lhs_it == *rhs_it))
        {
            return false;
        }

        ++lhs_it;
        ++rhs_it;
    }

    return true;
}

template<typename Key, typename Value, typename Comparer1, typename Comparer2>
bool operator!=(const SortedMap<Key, Value, Comparer1>& lhs, const SortedMap<Key, Value, Comparer2>& rhs)
{
    return not(lhs == rhs);
}
} // namespace Polly