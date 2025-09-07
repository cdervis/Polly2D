// Copyright (C) 2025 Cem Dervis
// This file is part of Polly, a 2D C++ game framework for minimalists.
// For conditions of distribution and use, see copyright notice in LICENSE, or https://polly2d.org.

#pragma once

#include "Polly/Maybe.hpp"
#include "Polly/Pair.hpp"
#include <initializer_list>

namespace Polly
{
/// Represents a doubly linked list of an arbitrary data type.
template<typename T>
class LinkedList
{
    struct Node
    {
        template<typename U>
        explicit constexpr Node(U&& value, Node* previous)
            : value(std::forward<U>(value))
            , previous(previous)
        {
            if (previous)
                previous->next = this;
        }

        T     value;
        Node* previous;
        Node* next = nullptr;
    };

  public:
    using value_type                         = T;
    static constexpr auto isForwardContainer = true;

    template<typename NodePointer>
    struct Iterator
    {
        constexpr Iterator()
            : node(nullptr)
        {
        }

        constexpr explicit Iterator(NodePointer n)
            : node(n)
        {
        }

        constexpr Iterator& operator++()
        {
            assumeWithMsg(node, "Attempting to advance an empty LinkedList iterator.");
            node = node->next;
            return *this;
        }

        constexpr Iterator operator++(int count)
        {
            auto it = *this;
            it += count;
            return it;
        }

        constexpr Iterator& operator+=(int count)
        {
            auto it = *this;

            while (count > 0)
            {
                assume(node);
                ++it;
                --count;
            }

            node = it->_node;

            return *this;
        }

        constexpr T& operator*()
        {
            return node->value;
        }

        constexpr const T& operator*() const
        {
            return node->value;
        }

        constexpr T* operator->()
        {
            return std::addressof(node->value);
        }

        constexpr const T* operator->() const
        {
            return std::addressof(node->value);
        }

        bool operator==(const Iterator& rhs) const
        {
            return node == rhs.node;
        }

        bool operator!=(const Iterator& rhs) const
        {
            return node != rhs.node;
        }

        NodePointer node;
    };

    LinkedList()
        : _front(nullptr)
        , _back(nullptr)
        , _size(0)
    {
    }

    LinkedList(const std::initializer_list<T>& items)
        : LinkedList()
    {
        addRange(items);
    }

    LinkedList(const LinkedList& copyFrom)
        : LinkedList()
    {
        assignFromList(copyFrom);
    }

    LinkedList& operator=(const LinkedList& copyFrom)
    {
        if (&copyFrom != this)
        {
            clear();
            assignFromList(copyFrom);
        }

        return *this;
    }

    LinkedList(LinkedList&& moveFrom) noexcept
        : _front(std::exchange(moveFrom._front, nullptr))
        , _back(std::exchange(moveFrom._back, nullptr))
        , _size(std::exchange(moveFrom._size, 0))
    {
    }

    LinkedList& operator=(LinkedList&& moveFrom) noexcept
    {
        if (&moveFrom != this)
        {
            clear();
            _front = std::exchange(moveFrom._front, nullptr);
            _back  = std::exchange(moveFrom._back, nullptr);
            _size  = std::exchange(moveFrom._size, 0);
        }

        return *this;
    }

    ~LinkedList() noexcept
    {
        clear();
    }

    template<typename U>
    void add(U&& value)
    {
        emplace(std::forward<U>(value));
    }

    template<typename Container>
    void addRange(const Container& container)
    {
        for (const auto& value : container)
        {
            emplace(value);
        }
    }

    template<typename... Args>
    T& emplace(Args&&... args)
    {
        auto* n = new Node(T(std::forward<Args>(args)...), _back);

        if (!_front)
        {
            _front = n;
        }

        _back = n;
        ++_size;

        return n->value;
    }

    [[nodiscard]]
    Maybe<u32> indexOf(const T& value) const
    {
        const auto [node, idx] = findNode(value);

        return node ? idx : none;
    }

    template<typename U>
    bool remove(const U& value)
    {
        if (auto [node, _] = findNode(value); node != nullptr)
        {
            destroyNode<true, true>(node);
            return true;
        }

        return false;
    }

    void clear()
    {
        auto* node = _front;

        while (node)
        {
            const auto next = node->next;
            destroyNode<false, false>(node);
            node = next;
        }

        _front = nullptr;
        _back  = nullptr;
        _size  = 0;
    }

    auto begin()
    {
        return Iterator<Node*>(_front);
    }

    auto begin() const
    {
        return Iterator<const Node*>(_front);
    }

    auto end()
    {
        return Iterator<Node*>(nullptr);
    }

    auto end() const
    {
        return Iterator<const Node*>(nullptr);
    }

  private:
    template<bool RelinkNeighbors, bool CountSize>
    void destroyNode(Node* node)
    {
        if constexpr (RelinkNeighbors)
        {
            node->previous->next = node->next;
            node->next->previous = node->previous;
        }

        delete node;

        if constexpr (CountSize)
        {
            if (--_size == 0)
            {
                _front = _back = nullptr;
            }
        }
    }

    Pair<Node*, u32> findNode(const T& value)
    {
        auto  idx  = 0u;
        auto* node = _front;

        while (node)
        {
            if (node->value == value)
            {
                return pair(node, idx);
            }

            node = node->next;
            ++idx;
        }

        return Pair(nullptr, u32(-1));
    }

    void assignFromList(const LinkedList& copyFrom)
    {
        for (const auto& value : copyFrom)
        {
            add(value);
        }
    }

    Node* _front;
    Node* _back;
    u32   _size;
};
} // namespace Polly
