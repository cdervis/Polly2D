// Copyright (C) 2025 Cemalettin Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE.

#pragma once

#include "Polly/Assume.hpp"
#include "Polly/CopyMoveMacros.hpp"
#include "Polly/Details/ListData.hpp"
#include <algorithm>
#include <memory>

namespace Polly::Details
{
template<typename T, u32 InlineCapacity>
class ListBase
{
  public:
    template<typename SameAllocator, u32 DifferentInlineCapacity>
    friend class ListBase;

  protected:
    [[nodiscard]]
    static consteval size_t inlineCapacity()
    {
        return InlineCapacity;
    }

    [[nodiscard]]
    T* ptrCast(const T* it)
    {
        return beginPtr() + (it - beginPtr());
    }

  private:
    class StackTemporary
    {
      public:
        StackTemporary() = delete;

        template<typename... Args>
        explicit StackTemporary( // NOLINT(*-pro-type-member-init)
            ListBase& list_base,
            Args&&... args)
            : _list_base(list_base)
        {
            std::construct_at(pointer(), std::forward<Args>(args)...);
        }

        deleteCopyAndMove(StackTemporary);

        ~StackTemporary()
        {
            std::destroy_at(pointer());
        }

        [[nodiscard]]
        const T& get() const
        {
            return *pointer();
        }

        [[nodiscard]]
        T&& release()
        {
            return std::move(*pointer());
        }

      private:
        [[nodiscard]]
        const T* pointer() const
        {
            return static_cast<const T*>(std::addressof(_data));
        }

        [[nodiscard]]
        T* pointer()
        {
            return static_cast<T*>(std::addressof(_data));
        }

        ListBase& _list_base;
        alignas(alignof(T)) unsigned char _data[sizeof(T)];
    };

    void wipe()
    {
        destroyRange(beginPtr(), endPtr());

        if (hasAllocation())
        {
            std::free(dataPtr()); // NOLINT(*-no-malloc)
        }
    }

    void setDataPtr(T* dataPtr)
    {
        _data.setDataPtr(dataPtr);
    }

    void setCapacity(u32 capacity)
    {
        assumeWithMsg(InlineCapacity <= capacity, "'capacity' must be greater than InlineCapacity.");
        _data.setCapacity(capacity);
    }

    void setSize(u32 size)
    {
        _data.setSize(size);
    }

    void setData(T* dataPtr, u32 capacity, u32 size)
    {
        _data.set(dataPtr, capacity, size);
    }

    void resetData(T* dataPtr, u32 capacity, u32 size)
    {
        wipe();
        _data.set(dataPtr, capacity, size);
    }

    void increaseSize(u32 n)
    {
        _data.setSize(size() + n);
    }

    void decreaseSize(u32 n)
    {
        _data.setSize(size() - n);
    }

    static T* allocateBuffer(u32 n)
    {
        assumeWithMsg(InlineCapacity < n, "Allocated capacity should be greater than InlineCapacity.");
        return static_cast<T*>(std::malloc(sizeof(T) * n)); // NOLINT(*-no-malloc)
    }

  protected:
    [[nodiscard]]
    static u32 calculateNewCapacity(u32 current, u32 required)
    {
        assume(current < required);

        const auto new_capacity = 2 * current;
        if (new_capacity < required)
        {
            return required;
        }

        return new_capacity;
    }

    [[nodiscard]]
    u32 calculateNewCapacity(u32 minimum_required_capacity) const
    {
        return calculateNewCapacity(capacity(), minimum_required_capacity);
    }

    template<typename ForwardIt>
    void overwriteExistingElements(const ForwardIt first, const ForwardIt last, const u32 count)
    {
        // The `count` parameter is just an optimization, since it may already be calculated by the
        // parent. This is important because it may be O(n) to compute the range size.
        assumeWithMsg(count <= capacity(), "Not enough capacity.");

        if (size() < count)
        {
            // There are more elements in the range.
            // Overwrite the existing range and uninitialized-copy the rest.
            ForwardIt pivot = copyNReturnIn(first, size(), beginPtr());
            uninitializedCopy(pivot, last, endPtr());
        }
        else
        {
            // There are fewer elements in `other`.
            // Overwrite part of the existing range and destroy the rest
            auto* new_end = copyRange(first, last, beginPtr());
            destroyRange(new_end, endPtr());
        }
    }

    template<typename Iterator>
    T* uninitializedCopy(Iterator first, Iterator last, T* d_first)
    {
        auto d_last = d_first;

        for (; first != last; ++first, ++d_last)
        {
            std::construct_at(d_last, *first);
        }

        return d_last;
    }

    void destroyRange(T* first, T* last)
    {
        for (; not(first == last); ++first)
        {
            std::destroy_at(first);
        }
    }

    T* defaultUninitializedValueConstruct(T* first, T* last)
    {
        T* curr = first;

        for (; not(curr == last); ++curr)
        {
            std::construct_at(curr);
        }

        return curr;
    }

    static T* uninitializedValueConstruct(T* first, T* last)
    {
        while (first != last)
        {
            *first = T();
            ++first;
        }

        return last;
    }

    template<u32 N, typename U = T>
    ListBase& copyAssign(const ListBase<U, N>& other)
    {
        assignWithIterators(other.beginPtr(), other.endPtr(), other.size());
        return *this;
    }

    template<u32 N>
    ListBase& moveAssign(ListBase<T, N>& other)
    {
        if (other.hasAllocation() and other.capacity() > InlineCapacity)
        {
            resetData(other.beginPtr(), other.capacity(), other.size());
            other.setDefault();
        }
        else
        {
            assignWithIterators(
                std::make_move_iterator(other.beginPtr()),
                std::make_move_iterator(other.endPtr()),
                other.size());
        }

        return *this;
    }

    template<u32 I = InlineCapacity>
    requires(I == 0)
    void moveInitialize(ListBase&& other) // NOLINT(*-rvalue-reference-param-not-moved)
    {
        setData(other.dataPtr(), other.capacity(), other.size());
        other.setDefault();
    }

    template<u32 LessEqualI>
    requires(LessEqualI <= InlineCapacity)
    void moveInitialize(ListBase<T, LessEqualI>&& other) // NOLINT(*-rvalue-reference-param-not-moved)
    {
        if (other.capacity() > InlineCapacity)
        {
            setData(other.dataPtr(), other.capacity(), other.size());
            other.setDefault();
        }
        else
        {
            setToInlineStorage();
            uninitializedMove(other.beginPtr(), other.endPtr(), dataPtr());
            setSize(other.size());
        }
    }

    template<u32 GreaterI>
    requires(GreaterI > InlineCapacity)
    void moveInitialize(ListBase<T, GreaterI>&& other) // NOLINT(*-rvalue-reference-param-not-moved)
    {
        if (other.hasAllocation())
        {
            setData(other.dataPtr(), other.capacity(), other.size());
            other.setDefault();
        }
        else
        {
            if (InlineCapacity < other.size())
            {
                // We may throw in this case.
                setDataPtr(UncheckedAllocate(other.size(), other.allocationEndPtr()));
                setCapacity(other.size());

                uninitializedMove(other.beginPtr(), other.endPtr(), dataPtr());
            }
            else
            {
                setToInlineStorage();
                uninitializedMove(other.beginPtr(), other.endPtr(), dataPtr());
            }

            setSize(other.size());
        }
    }

  public:
    deleteCopyAndMove(ListBase);

    ListBase()
    {
        setDefault();
    }

    static constexpr struct BypassTag
    {
    } bypass{};

    template<u32 I>
    ListBase(BypassTag, const ListBase<T, I>& other)
    {
        if (InlineCapacity < other.size())
        {
            setDataPtr(allocateBuffer(other.size()));
            setCapacity(other.size());

            uninitializedCopy(other.beginPtr(), other.endPtr(), dataPtr());
        }
        else
        {
            setToInlineStorage();
            uninitializedCopy(other.beginPtr(), other.endPtr(), dataPtr());
        }

        setSize(other.size());
    }

    template<u32 I>
    ListBase(BypassTag, ListBase<T, I>&& other)
    {
        moveInitialize(std::move(other));
    }

    explicit ListBase(u32 count)
    {
        if (InlineCapacity < count)
        {
            setDataPtr(allocateBuffer(count));
            setCapacity(count);
        }
        else
        {
            setToInlineStorage();
        }

        uninitializedValueConstruct(beginPtr(), beginPtr() + count);
        setSize(count);
    }

    ListBase(u32 count, const T& val)
    {
        if (InlineCapacity < count)
        {
            setDataPtr(allocateBuffer(count));
            setCapacity(count);
        }
        else
        {
            setToInlineStorage();
        }

        uninitializedFill(beginPtr(), beginPtr() + count, val);
        setSize(count);
    }

    ~ListBase()
    {
        assume(InlineCapacity <= capacity() and "Invalid capacity.");
        wipe();
    }

  protected:
    void setToInlineStorage()
    {
        setCapacity(InlineCapacity);
        setDataPtr(storagePtr());
    }

    void assignWithCopies(u32 count, const T& val)
    {
        if (capacity() < count)
        {
            const auto new_capacity = calculateNewCapacity(count);
            auto       new_begin    = allocateBuffer(new_capacity);
            uninitializedFill(new_begin, new_begin + count, val);
            resetData(new_begin, new_capacity, count);
        }
        else if (size() < count)
        {
            std::fill(beginPtr(), endPtr(), val);
            uninitializedFill(endPtr(), beginPtr() + count, val);
            setSize(count);
        }
        else
        {
            eraseRange(std::fill_n(beginPtr(), count, val), endPtr());
        }
    }

    template<typename Container>
    void assignWithRange(Container&& container)
    {
        assignWithIterators(container.begin(), container.end(), static_cast<u32>(container.size()));
    }

    template<typename Iterator>
    void assignWithIterators(Iterator first, Iterator last, u32 count)
    {
        if (count > capacity())
        {
            // More elements are being added than we can currently holy; grow the buffer.
            const auto new_capacity = calculateNewCapacity(count);
            auto*      new_begin    = allocateBuffer(new_capacity);

            uninitializedCopy(first, last, new_begin);

            wipe();
            setDataPtr(new_begin);
            setCapacity(new_capacity);
        }
        else if (count <= InlineCapacity and hasAllocation())
        {
            // We're on the heap, but the elements can fit in our small buffer.
            // Therefore destroy our heap buffer and move the elements into the small buffer.
            auto* new_begin = storagePtr();
            uninitializedCopy(first, last, new_begin);
            destroyRange(beginPtr(), endPtr());
            std::free(beginPtr()); // NOLINT(*-no-malloc)
            setDataPtr(new_begin);
            setCapacity(InlineCapacity);
        }
        else
        {
            // We're small and everything fits into the small buffer.
            overwriteExistingElements(first, last, count);
        }

        setSize(count);
    }

    T* uninitializedMove(T* first, T* last, T* d_first)
    {
        auto d_last = d_first;

        for (; first != last; ++first, static_cast<void>(++d_last))
        {
            std::construct_at(d_last, std::move(*first));
        }

        return d_last;
    }

    T* shiftIntoUninitialized(T* pos, u32 shift_count)
    {
        // Shift elements over to the right into uninitialized space.
        // Returns the start of the shifted range.
        // Precondition: shift < GetEndPtr () - pos
        assume(shift_count != 0);

        const auto original_end = endPtr();
        const auto pivot        = original_end - shift_count;

        uninitializedMove(pivot, original_end, original_end);
        increaseSize(shift_count);

        return moveRight(pos, pivot, original_end);
    }

    template<typename... Args>
    T* appendElement(Args&&... args)
    {
        if (size() < capacity())
        {
            return emplaceIntoCurrentEnd(std::forward<Args>(args)...);
        }

        return emplaceIntoReallocationEnd(std::forward<Args>(args)...);
    }

    T* appendCopies(u32 count, const T& val)
    {
        if (uninitializedCount() < count)
        {
            auto       original_size = size();
            const auto new_size      = size() + count;
            const auto new_capacity  = calculateNewCapacity(new_size);
            auto       new_data_ptr  = allocateBuffer(new_capacity, allocationEndPtr());
            auto       new_last      = new_data_ptr + original_size;

            uninitializedFill(new_last, new_last + count, val);
            uninitializedMove(beginPtr(), endPtr(), new_data_ptr);

            resetData(new_data_ptr, new_capacity, new_size);
            return new_data_ptr + original_size;
        }

        const auto ret = endPtr();
        uninitializedFill(ret, ret + count, val);
        increaseSize(count);

        return ret;
    }

    template<typename Container>
    T* appendRange(Container&& container)
    {
        const auto original_size = size();

        for (auto&& element : container)
        {
            appendElement(std::forward<decltype(element)&&>(element));
        }

        return beginPtr() + original_size;
    }

    template<typename... Args>
    T* emplaceAt(T* pos, Args&&... args)
    {
        assume(size() <= capacity() and "size was greater than capacity");

        if (size() < capacity())
        {
            return emplaceIntoCurrent(pos, std::forward<Args>(args)...);
        }

        return emplaceIntoReallocation(pos, std::forward<Args>(args)...);
    }

    T* insertCopies(T* pos, u32 count, const T& val)
    {
        if (count == 0)
        {
            return pos;
        }

        if (endPtr() == pos)
        {
            if (count == 1)
            {
                return appendElement(val);
            }

            return appendCopies(count, val);
        }

        if (uninitializedCount() < count)
        {
            // Reallocate.
            const auto offset   = static_cast<u32>(pos - beginPtr());
            const auto new_size = size() + count;

            // The check is handled by the if-guard.
            const auto new_capacity = calculateNewCapacity(new_size);
            auto       new_data_ptr = UncheckedAllocate(new_capacity, allocationEndPtr());
            auto       new_first    = new_data_ptr + offset;
            auto       new_last     = new_first;

            uninitializedFill(new_first, new_first + count, val);
            new_last += count;

            uninitializedMove(beginPtr(), pos, new_data_ptr);
            uninitializedMove(pos, endPtr(), new_last);

            resetData(new_data_ptr, new_capacity, new_size);

            return beginPtr() + offset;
        }

        // If we have fewer to insert than tailing elements after `pos`, we shift into
        // uninitialized and then copy over.

        if (const auto tail_size = static_cast<u32>(endPtr() - pos); tail_size < count)
        {
            // The number inserted is larger than the number after `pos`,
            // so part of the input will be used to construct new elements,
            // and another part of it will assign existing ones.
            // In order:
            //   Construct new elements immediately after GetEndPtr () using the input.
            //   Move-construct existing elements over to the tail.
            //   Assign existing elements using the input.

            auto* original_end = endPtr();

            // Place a portion of the input into the uninitialized section.
            auto num_val_tail = count - tail_size;

            uninitializedFill(endPtr(), endPtr() + num_val_tail, val);
            increaseSize(num_val_tail);

            // We need to handle possible aliasing here.
            const auto tmp = StackTemporary(*this, val);

            // Now, move the tail to the end.
            uninitializedMove(pos, original_end, endPtr());
            increaseSize(tail_size);

            // Finally, try to copy the rest of the elements over.
            std::fill_n(pos, tail_size, tmp.get());
        }
        else
        {
            const auto tmp          = StackTemporary(*this, val);
            auto       inserted_end = shiftIntoUninitialized(pos, count);
            std::fill(pos, inserted_end, tmp.get());
        }

        return pos;
    }

    template<Concepts::ForwardContainer Container>
    T* insertRangeHelper(T* pos, const Container& container)
    {
        assumeWithMsg(not container.isEmpty(), "The range should not be empty.");

        const auto num_insert = container.size();

        if (uninitializedCount() < num_insert)
        {
            // Reallocate.
            const auto offset   = static_cast<u32>(pos - beginPtr());
            const auto new_size = size() + num_insert;

            // The check is handled by the if-guard.
            const auto new_capacity = calculateNewCapacity(new_size);
            const auto new_data_ptr = allocateBuffer(new_capacity);
            auto       new_first    = new_data_ptr + offset;
            auto       new_last     = new_first;

            uninitializedCopy(container.begin(), container.end(), new_first);
            new_last += num_insert;

            uninitializedMove(beginPtr(), pos, new_data_ptr);
            uninitializedMove(pos, endPtr(), new_last);

            resetData(new_data_ptr, new_capacity, new_size);

            return beginPtr() + offset;
        }

        // if we have fewer to insert than tailing elements after
        // `pos` we shift into uninitialized and then copy over
        if (const auto tail_size = static_cast<u32>(endPtr() - pos); tail_size < num_insert)
        {
            // Use the same method as InsertCopies.
            auto* original_end = endPtr();
            auto  pivot        = container.begin() + tail_size;

            // Place a portion of the input into the uninitialized section.
            uninitializedCopy(pivot, container.end(), endPtr());
            increaseSize(num_insert - tail_size);

            // Now move the tail to the end.
            uninitializedMove(pos, original_end, endPtr());
            increaseSize(tail_size);

            // Finally, try to copy the rest of the elements over.
            copyRange(container.begin(), pivot, pos);
        }
        else
        {
            shiftIntoUninitialized(pos, num_insert);

            // Attempt to copy over the elements.
            // If we fail we'll attempt a full roll-back.
            copyRange(container.begin(), container.end(), pos);
        }

        return pos;
    }

    template<Concepts::ForwardContainer Container>
    T* insertRange(T* pos, const Container& container)
    {
        if (not(endPtr() == pos))
        {
            return insertRangeHelper(pos, container);
        }

        if (container.size() == 1)
        {
            return appendElement(*container.begin());
        }

        return appendRange(container);
    }

    template<typename... Args>
    T* emplaceIntoCurrentEnd(Args&&... args)
    {
        std::construct_at(endPtr(), std::forward<Args>(args)...);
        increaseSize(1);
        return endPtr() - 1;
    }

    template<typename V = T, std::enable_if_t<std::is_nothrow_move_constructible_v<V>>* = nullptr>
    T* emplaceIntoCurrent(T* pos, T&& val)
    {
        if (pos == endPtr())
        {
            return emplaceIntoCurrentEnd(std::move(val));
        }

        // In the special case of T&& we don't make a copy because behavior is unspecified
        // when it is an internal element. Hence, we'll take the opportunity to optimize and assume
        // that it isn't an internal element.
        shiftIntoUninitialized(pos, 1);
        std::destroy_at(pos);
        std::construct_at(pos, std::move(val));

        return pos;
    }

    template<typename... Args>
    T* emplaceIntoCurrent(T* pos, Args&&... args)
    {
        if (pos == endPtr())
        {
            return emplaceIntoCurrentEnd(std::forward<Args>(args)...);
        }

        // This is necessary because of possible aliasing.
        auto tmp = StackTemporary(*this, std::forward<Args>(args)...);
        shiftIntoUninitialized(pos, 1);
        *pos = tmp.release();
        return pos;
    }

    template<typename... Args>
    T* emplaceIntoReallocationEnd(Args&&... args)
    {
        // Appending; strong exception guarantee.
        const auto new_size = size() + 1;

        // The check is handled by the if-guard.
        const auto new_capacity = calculateNewCapacity(new_size);
        const auto new_data_ptr = allocateBuffer(new_capacity);
        const auto emplace_pos  = new_data_ptr + size();

        std::construct_at(emplace_pos, std::forward<Args>(args)...);

        uninitializedMove(beginPtr(), endPtr(), new_data_ptr);

        resetData(new_data_ptr, new_capacity, new_size);

        return emplace_pos;
    }

    template<typename... Args>
    T* emplaceIntoReallocation(T* pos, Args&&... args)
    {
        const auto offset = static_cast<u32>(pos - beginPtr());

        if (offset == size())
        {
            return emplaceIntoReallocationEnd(std::forward<Args>(args)...);
        }

        const auto new_size     = size() + 1;
        const auto new_capacity = calculateNewCapacity(new_size);
        const auto new_data_ptr = allocateBuffer(new_capacity);
        auto       new_first    = new_data_ptr + offset;
        auto       new_last     = new_first;

        std::construct_at(new_first, std::forward<Args>(args)...);
        ++new_last;

        uninitializedMove(beginPtr(), pos, new_data_ptr);
        new_first = new_data_ptr;
        uninitializedMove(pos, endPtr(), new_last);

        resetData(new_data_ptr, new_capacity, new_size);

        return beginPtr() + offset;
    }

    T* shrinkToSize()
    {
        if (not hasAllocation() or size() == capacity())
        {
            return beginPtr();
        }

        // The rest runs only if allocated.

        auto newCapacity = static_cast<u32>(0);
        T*   newDataPtr  = nullptr;

        if (size() > InlineCapacity)
        {
            newCapacity = size();
            newDataPtr  = UncheckedAllocate(newCapacity, allocationEndPtr());
        }
        else
        {
            // We move to inline storage.
            newCapacity = InlineCapacity;
            newDataPtr  = storagePtr();
        }

        uninitializedMove(beginPtr(), endPtr(), newDataPtr);

        destroyRange(beginPtr(), endPtr());
        Deallocate(dataPtr(), capacity());

        setDataPtr(newDataPtr);
        setCapacity(newCapacity);

        return beginPtr();
    }

    template<typename... ValueT>
    void resizeWith(u32 newSize, const ValueT&... val)
    {
        // ValueT... should either be T or empty.

        if (newSize == 0)
        {
            eraseAll();
        }

        if (capacity() < newSize)
        {
            // Reallocate.

            const auto originalSize = size();

            // The check is handled by the if-guard.
            const auto newCapacity = calculateNewCapacity(newSize);
            auto*      newDataPtr  = allocateBuffer(newCapacity);
            auto*      newLast     = newDataPtr + originalSize;

            uninitializedFill(newLast, newDataPtr + newSize, val...);

            // Strong exception guarantee.
            uninitializedMove(beginPtr(), endPtr(), newDataPtr);

            resetData(newDataPtr, newCapacity, newSize);
        }
        else if (size() < newSize)
        {
            // Construct in the uninitialized section.
            uninitializedFill(endPtr(), beginPtr() + newSize, val...);
            setSize(newSize);
        }
        else
        {
            eraseRange(beginPtr() + newSize, endPtr());
        }

        // Do nothing if the count is the same as the current size.
    }

    T* uninitializedFill(T* first, T* last)
    {
        return uninitializedValueConstruct(first, last);
    }

    T* uninitializedFill(T* first, T* last, const T& val)
    {
        auto* curr = first;

        for (; not(curr == last); ++curr)
        {
            construct(curr, val);
        }

        return curr;
    }

    void requestCapacity(u32 request)
    {
        if (request <= capacity())
        {
            return;
        }

        const auto new_capacity = calculateNewCapacity(request);
        auto       new_begin    = allocateBuffer(new_capacity);

        uninitializedMove(beginPtr(), endPtr(), new_begin);

        wipe();

        setDataPtr(new_begin);
        setCapacity(new_capacity);
    }

    T* eraseAt(T* pos)
    {
        moveLeft(pos + 1, endPtr(), pos);
        eraseLast();
        return pos;
    }

    void eraseLast()
    {
        decreaseSize(1);

        // The element located at GetEndPtr is still alive since the size decreased.
        std::destroy_at(endPtr());
    }

    T* eraseRange(T* first, T* last)
    {
        if (not(first == last))
        {
            eraseToEnd(moveLeft(last, endPtr(), first));
        }

        return first;
    }

    void eraseToEnd(T* pos)
    {
        assumeWithMsg(0 <= endPtr() - pos, "`pos` was in the uninitialized range");

        if (const auto change = static_cast<u32>(endPtr() - pos))
        {
            decreaseSize(change);
            destroyRange(pos, pos + change);
        }
    }

    void eraseAll()
    {
        const auto current_end = endPtr();
        setSize(0);
        destroyRange(beginPtr(), current_end);
    }

    template<u32 N>
    void swapElements(ListBase<T, N>& other)
    {
        if (other.size() < size())
        {
            return other.swap_elements_equal_or_non_propagated_allocators(*this);
        }

        const auto* other_tail = std::swap_ranges(beginPtr(), endPtr(), other.beginPtr());
        uninitializedMove(other_tail, other.endPtr(), endPtr());
        other.destroyRange(other_tail, other.endPtr());
    }

    template<u32 N>
    requires(N == 0)
    void swapEqualOrPropagatedAllocators(ListBase& other)
    {
        _data.swap(other._data);
    }

    template<u32 LessEqualI>
    requires(LessEqualI <= InlineCapacity)
    void swapEqualOrPropagatedAllocators(ListBase<T, LessEqualI>& other)
    {
        if (hasAllocation())
        {
            if (InlineCapacity < other.capacity())
            {
                // Note: This is always the branch that will run when constant-evaluated.
                _data.swap_data_ptr(other._data);
                _data.swap_capacity(other._data);
            }
            else
            {
                // Move the elements of `other` into inline storage.
                // Give our pointer to `other`.
                auto* new_data_ptr = storagePtr();

                other.uninitializedMove(other.beginPtr(), other.endPtr(), new_data_ptr);

                other.wipe();
                other.setDataPtr(dataPtr());
                other.setCapacity(capacity());

                setDataPtr(new_data_ptr);
                setCapacity(InlineCapacity);
            }
        }
        else if (InlineCapacity < other.capacity())
        {
            // This implies that `other` is allocated, and that we can use its pointer.

            auto new_capacity = LessEqualI;
            auto new_data_ptr = other.storagePtr();

            // Check to see if we can store our elements in the inline storage of `other`.
            if (LessEqualI < size())
            {
                new_capacity = other.calculateNewCapacity(LessEqualI, size());
                new_data_ptr = allocate_with_hint(new_capacity, allocationEndPtr());

                uninitializedMove(beginPtr(), endPtr(), new_data_ptr);
            }
            else
            {
                uninitializedMove(beginPtr(), endPtr(), new_data_ptr);
            }

            destroyRange(beginPtr(), endPtr());

            setDataPtr(other.dataPtr());
            setCapacity(other.capacity());

            other.setDataPtr(new_data_ptr);
            other.setCapacity(new_capacity);
        }
        else if (LessEqualI < size())
        {
            // We have too many elements to store in `other`. allocate a new buffer.
            auto new_capacity = other.calculateNewCapacity(LessEqualI, size());
            auto new_data_ptr = allocate_with_hint(new_capacity, other.allocationEndPtr());

            auto new_end = uninitializedMove(beginPtr(), endPtr(), new_data_ptr);
            overwriteExistingElements(std::move(other));

            other.wipe();
            other.setDataPtr(new_data_ptr);
            other.setCapacity(new_capacity);
        }
        else if (other.hasAllocation())
        {
            // Move our elements into the inline storage of `other` (which is empty).
            // Move the elements of `other` into our inline storage.
            // Delete the allocation.
            uninitializedMove(beginPtr(), endPtr(), other.storagePtr());
            overwriteExistingElements(std::move(other));

            other.destroyRange(other.beginPtr(), other.endPtr());
            other.Deallocate(other.dataPtr(), other.capacity());
            other.setDataPtr(other.storagePtr());
            other.setCapacity(LessEqualI);
        }
        else
        {
            swapElements(other);
        }

        _data.swap_size(other._data);
    }

    template<u32 GreaterI>
    requires(GreaterI > InlineCapacity)
    void swap(ListBase<T, GreaterI>& other)
    {
        return other.swap(*this);
    }

    template<u32 LessEqualI, std::enable_if<LessEqualI <= InlineCapacity>>
    void swap(ListBase<T, LessEqualI>& other)
    {
        return swapEqualOrPropagatedAllocators(other);
    }

    template<typename InputIt>
    T* copyRange(InputIt first, InputIt last, T* dest)
    {
        return std::copy(first, last, dest);
    }

    template<typename InputIt>
    static InputIt copyNReturnIn(InputIt first, u32 count, T* dest)
    {
        for (; count != 0; --count, static_cast<void>(++dest), static_cast<void>(++first))
        {
            *dest = *first;
        }

        return first;
    }

    T* moveLeft(T* first, T* last, T* d_first)
    {
        // Shift initialized elements to the left.
        return std::move(first, last, d_first);
    }

    T* moveRight(T* first, T* last, T* d_last)
    {
        // move initialized elements to the right
        // n should not be 0
        return std::move_backward(first, last, d_last);
    }

  public:
    void setDefault()
    {
        setToInlineStorage();
        setSize(0);
    }

    void releaseData()
    {
        _data.release();
    }

    T* dataPtr()
    {
        return _data.dataPtr();
    }

    const T* dataPtr() const
    {
        return _data.dataPtr();
    }

    u32 capacity() const
    {
        return _data.capacity();
    }

    u32 size() const
    {
        return _data.size();
    }

    u32 uninitializedCount() const
    {
        return capacity() - size();
    }

    T* beginPtr()
    {
        return dataPtr();
    }

    const T* beginPtr() const
    {
        return dataPtr();
    }

    T* endPtr()
    {
        return beginPtr() + size();
    }

    const T* endPtr() const
    {
        return beginPtr() + size();
    }

    T* allocationEndPtr()
    {
        return beginPtr() + capacity();
    }

    const T* allocationEndPtr() const
    {
        return beginPtr() + capacity();
    }

    T* storagePtr()
    {
        return _data.storage();
    }

    bool hasAllocation() const
    {
        return InlineCapacity < capacity();
    }

  private:
    ListData<T, InlineCapacity> _data;
};
} // namespace Polly::Details
