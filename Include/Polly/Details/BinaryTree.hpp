// Copyright (C) 2025 Cem Dervis
// This file is part of Polly, a minimalistic 2D C++ game framework.
// For conditions of distribution and use, see copyright notice in LICENSE, or https://polly2d.org.

#pragma once

#include "Polly/Assume.hpp"
#include "Polly/Maybe.hpp"
#include <concepts>
#include <functional>

namespace Polly::Details
{
template<typename T, typename Comparer>
class BinaryTree
{
  public:
    struct Node
    {
        template<typename U>
        requires(std::convertible_to<U, T>)
        explicit constexpr Node(U&& value, Node* parent)
            : value(std::forward<U&&>(value))
            , parent(parent)
        {
        }

        T     value;
        Node* parent;
        Node* left  = nullptr;
        Node* right = nullptr;
    };

    template<typename NodePointer>
    struct Iterator
    {
        friend BinaryTree;

        constexpr Iterator()
            : _node(nullptr)
        {
        }

        constexpr explicit Iterator(NodePointer n)
            : _node(n)
        {
        }

        constexpr Iterator& operator++()
        {
            assumeWithMsg(_node, "Attempting to advance an empty BinaryTree iterator.");

            if (_nextIsLeft and _node->left)
            {
                _node = _node->left;
            }
            else if (_node->right)
            {
                _node = _node->right;

                while (_node and _node->left)
                {
                    _node = _node->left;
                }

                _nextIsLeft = true;
            }
            else if (_node->parent)
            {
                // Have to track back to the parent and continue with the successor node.
                while (_node and _node->parent and _node == _node->parent->right)
                {
                    _node = _node->parent;
                }

                if (_node and _node->parent)
                {
                    _node = _node->parent;
                }
                else
                {
                    _node = nullptr;
                }

                _nextIsLeft = false;
            }
            else
            {
                _node = nullptr;
            }

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
                assumeWithMsg(_node, "unexpected null-node during iteration");
                ++it;
                --count;
            }

            _node = it->_node;

            return *this;
        }

        constexpr const T& operator*() const
        {
            return _node->value;
        }

        constexpr const T* operator->() const
        {
            return std::addressof(_node->value);
        }

        bool operator==(const Iterator& rhs) const
        {
            return _node == rhs._node;
        }

        bool operator!=(const Iterator& rhs) const
        {
            return _node != rhs._node;
        }

      private:
        NodePointer _node;
        bool        _nextIsLeft = true;
    };

    constexpr BinaryTree()
        : rootNode(nullptr)
        , _size(0)
    {
    }

    constexpr BinaryTree(const BinaryTree& copyFrom)
        : BinaryTree()
    {
        addRange(copyFrom);
    }

    constexpr BinaryTree& operator=(const BinaryTree& copyFrom)
    {
        if (&copyFrom != this)
        {
            clear();
            addRange(copyFrom);
        }

        return *this;
    }

    constexpr BinaryTree(BinaryTree&& moveFrom) noexcept
        : rootNode(std::exchange(moveFrom.rootNode, nullptr))
        , _size(std::exchange(moveFrom._size, 0))
    {
    }

    constexpr BinaryTree& operator=(BinaryTree&& moveFrom) noexcept
    {
        if (&moveFrom != this)
        {
            clear();
            rootNode = std::exchange(moveFrom.rootNode, nullptr);
            _size    = std::exchange(moveFrom._size, 0);
        }

        return *this;
    }

    ~BinaryTree() noexcept
    {
        clear();
    }

    template<typename U>
    Maybe<Node&> add(U&& value)
    {
        if (not rootNode)
        {
            rootNode = new Node(std::forward<U>(value), nullptr);
            ++_size;
            return *rootNode;
        }

        auto* node = rootNode;

        const auto comparer = Comparer();

        while (node)
        {
            // Select the next node based on whether the specified value is
            // less than or greater than the current node's value.
            if (comparer(value, node->value))
            {
                if (not node->left)
                {
                    node->left = new Node(std::forward<U>(value), node);
                    ++_size;
                    return *node->left;
                }

                node = node->left;
            }
            else if (comparer(node->value, value))
            {
                if (not node->right)
                {
                    node->right = new Node(std::forward<U>(value), node);
                    ++_size;
                    return *node->right;
                }

                node = node->right;
            }
            else
            {
                // Value already exists
                break;
            }
        }

        return none;
    }

    template<typename Container>
    void addRange(const Container& container)
    {
        for (const auto& value : container)
        {
            add(value);
        }
    }

    void clear()
    {
        destroyNodesRecursively<false>(rootNode);
        _size = 0;
    }

    template<typename U>
    bool remove(const U& value)
    {
        if (auto* node = findNode(value))
        {
            destroyNode(node);
            return true;
        }

        return false;
    }

    template<typename Container>
    u32 removeRange(const Container& container)
    {
        auto removedCount = 0u;

        for (const auto& value : container)
        {
            if (remove(value))
            {
                ++removedCount;
            }
        }

        return removedCount;
    }

    template<typename Predicate>
    u32 removeWhere(Predicate&& predicate)
    {
        auto it           = begin();
        auto en           = end();
        auto removedCount = 0u;

        while (it != en)
        {
            if (std::invoke(predicate, *it))
            {
                destroyNode(it._node);
                it = begin();
                ++removedCount;
            }
            else
            {
                ++it;
            }
        }

        return removedCount;
    }

    u32 size() const
    {
        return _size;
    }

    Iterator<Node*> begin()
    {
        return Iterator<Node*>(leftMostNode(rootNode));
    }

    Iterator<const Node*> begin() const
    {
        return Iterator<const Node*>(leftMostNode(rootNode));
    }

    Iterator<Node*> end()
    {
        return Iterator<Node*>();
    }

    Iterator<const Node*> end() const
    {
        return Iterator<const Node*>();
    }

    Node* leftMostNode()
    {
        return leftMostNode(rootNode);
    }

    const Node* leftMostNode() const
    {
        return leftMostNode(rootNode);
    }

    Node* rightMostNode()
    {
        return rightMostNode(rootNode);
    }

    const Node* rightMostNode() const
    {
        return rightMostNode(rootNode);
    }

    template<typename Comp = Comparer, typename U>
    Node* findNode(const U& value) const
    {
        auto* node = rootNode;

        const auto comparer = Comp();

        while (node)
        {
            if (comparer(value, node->value))
            {
                node = node->left;
            }
            else if (comparer(node->value, value))
            {
                node = node->right;
            }
            else
            {
                return node;
            }
        }

        return nullptr;
    }

    template<typename Predicate>
    const Node* findNodeWhere(Predicate&& predicate) const
    {
        return findNodeWhere(rootNode, std::forward<Predicate>(predicate));
    }

    template<typename Predicate>
    static const Node* findNodeWhere(const Node* node, Predicate&& predicate)
    {
        while (node)
        {
            if (predicate(node->value))
            {
                return node;
            }

            if (node->left)
            {
                if (const auto* n = findNodeWhere(node->left, std::forward<Predicate>(predicate)))
                {
                    return n;
                }
            }

            if (node->right)
            {
                if (const auto* n = findNodeWhere(node->right, std::forward<Predicate>(predicate)))
                {
                    return n;
                }
            }
        }

        return nullptr;
    }

    void destroyNode(Node* node)
    {
        reparentTreeForNodeRemoval(node);

        if (node == rootNode)
        {
            rootNode = node->left ? node->left : node->right;
        }

        delete node;

        if (--_size == 0)
        {
            rootNode = nullptr;
        }
    }

  private:
    void reparentTreeForNodeRemoval(Node* node)
    {
        // There are 3 possible cases:
        //   1) The node is a leaf (has no children)
        //   2) The node has only one child
        //   3) The node has two children

        if (node->left and node->right)
        {
            // Case 3
            auto* successor = node->left;

            while (successor and successor->right)
            {
                successor = successor->right;
            }

            if (successor->left or successor->right)
            {
                reparentTreeForNodeRemoval(successor);
            }
            else
            {
                reparentNode<false>(successor, nullptr);
            }

            successor->parent = node->parent;
            successor->left   = node->left;
            successor->right  = node->right;

            if (successor->left)
            {
                successor->left->parent = successor;
            }

            if (successor->right)
            {
                successor->right->parent = successor;
            }

            if (not successor->parent)
            {
                rootNode = successor;
            }
            else if (node->parent == rootNode)
            {
                reparentNode<false>(node, successor);
            }
        }
        else if (not node->left and not node->right)
        {
            // Case 1
            reparentNode<true>(node, nullptr);
        }
        else
        {
            // Case 2
            auto* childNode = node->left ? node->left : node->right;
            reparentNode<true>(node, childNode);
            childNode->parent = node->parent;
        }
    }

    template<bool CheckParentNull>
    void reparentNode(Node* child, Node* newNode)
    {
        auto* parent = child->parent;

        if constexpr (CheckParentNull)
        {
            if (not parent)
            {
                return;
            }
        }

        if (child == parent->left)
        {
            parent->left = newNode;
        }
        else
        {
            parent->right = newNode;
        }
    }

    template<bool Counting>
    void destroyNodesRecursively(Node* node)
    {
        if (not node)
        {
            return;
        }

        if (node->left)
        {
            destroyNodesRecursively<Counting>(node->left);
        }

        if (node->right)
        {
            destroyNodesRecursively<Counting>(node->right);
        }

        destroyNode(node);

        if constexpr (Counting)
        {
            --_size;
        }
    }

    template<typename NodePtr>
    static NodePtr leftMostNode(NodePtr node)
    {
        while (node and node->left)
        {
            node = node->left;
        }

        return node;
    }

    template<typename NodePtr>
    static NodePtr rightMostNode(NodePtr node)
    {
        while (node and node->right)
        {
            node = node->right;
        }

        return node;
    }

    Node* rootNode;
    u32   _size; // NOLINT(*-use-default-member-init)
};
} // namespace Polly::Details
