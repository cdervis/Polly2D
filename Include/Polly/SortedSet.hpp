// Copyright (C) 2025 Cem Dervis
// This file is part of Polly, a minimalistic 2D C++ game framework.
// For conditions of distribution and use, see copyright notice in LICENSE, or https://polly2d.org.

#pragma once

#include "Polly/Comparers.hpp"
#include "Polly/Concepts.hpp"
#include "Polly/Details/BinaryTree.hpp"
#include "Polly/Prerequisites.hpp"
#include "Polly/Span.hpp"

namespace Polly
{
namespace Details
{
[[noreturn]]
void throwSortedSetEmptyException();
}

/// Represents a set of unique objects that remain in order.
///
/// Comparison is done via operator< between two objects of type T.
///
/// @tparam T The type of elements stored in the set. Must support operator<.
template<typename T, typename Comparer = Comparers::Less<T>>
class SortedSet
{
  public:
    using value_type                         = T;
    static constexpr auto isForwardContainer = true;

    SortedSet() = default;

    SortedSet(Span<T> values)
        : SortedSet()
    {
        addRange(values);
    }

    template<typename U>
    requires(std::convertible_to<U, T>)
    bool add(U&& value)
    {
        return bool(_tree.add(std::forward<U>(value)));
    }

    template<typename Container>
    void addRange(Container&& container)
    {
        _tree.addRange(std::forward<Container>(container));
    }

    bool contains(const T& value) const
    {
        return _tree.findNode(value) != nullptr;
    }

    void clear()
    {
        _tree.clear();
    }

    bool remove(const T& value)
    {
        return _tree.remove(value);
    }

    template<Concepts::ForwardContainer Container>
    u32 remove(const Container& container)
    {
        return _tree.removeRange(container);
    }

    template<typename Predicate>
    u32 removeWhere(Predicate&& predicate)
    {
        return _tree.removeWhere(std::forward<Predicate>(predicate));
    }

    const T& minimumValue() const
    {
        checkEmptyAccess();
        return _tree.leftMostNode()->value;
    }

    const T& maximumValue() const
    {
        checkEmptyAccess();
        return _tree.rightMostNode()->value;
    }

    bool isEmpty() const
    {
        return size() == 0;
    }

    bool size() const
    {
        return _tree.size();
    }

    auto begin() const
    {
        return _tree.begin();
    }

    auto end() const
    {
        return _tree.end();
    }

  private:
    void checkEmptyAccess() const
    {
#ifndef polly_no_hardening
        if (isEmpty())
        {
            Details::throwSortedSetEmptyException();
        }
#endif
    }

    Details::BinaryTree<T, Comparer> _tree;
};
} // namespace Polly