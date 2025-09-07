// Copyright (C) 2025 Cem Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE, or https://polly2d.org.

#include "Polly/Any.hpp"
#include "Polly/Array.hpp"
#include "Polly/ByteBlob.hpp"
#include "Polly/Error.hpp"
#include "Polly/Format.hpp"
#include "Polly/Maybe.hpp"
#include "Polly/NotNull.hpp"
#include "Polly/SortedMap.hpp"
#include "Polly/SortedSet.hpp"
#include "Polly/StringView.hpp"
#include "Polly/UniquePtr.hpp"

void Polly::Details::throwEmptyUniquePtrDerefException()
{
    throw Error("Attempting to dereference an empty unique_ptr.");
}

void Polly::Details::throwKeyNotFoundException(StringView key)
{
    if (key.isEmpty())
    {
        throw Error("Attempting to access a SortedMap value by a non-existent key.");
    }

    throw Error(formatString("Attempting to access a SortedMap value by non-existent key '{}'.", key));
}

void Polly::Details::throwEmptyStringViewException()
{
    throw Error("Attempting to access characters of an empty StringView");
}

void Polly::Details::throwStringViewOutOfRangeException()
{
    throw Error("Index out of StringView range");
}

void Polly::Details::throwStringViewRangeException()
{
    throw Error("Index out of StringView range");
}

void Polly::Details::throwStringViewNotNullTerminatedException()
{
    throw Error("Attempting to get the C-string of a StringView that is not null-terminated.");
}

void Polly::Details::throwByteBlobIndexOutOfRangeException(u32 index, u32 size)
{
    if (size == 0)
    {
        throw Error(formatString("Attempting to access an empty ByteBlob object at index {}.", index));
    }

    throw Error(formatString(
        "Attempting to access a ByteBlob with size {} at index {} (out of range).",
        size,
        index));
}

void Polly::Details::throwInvalidNotNullConstructionException()
{
    throw Error("Attempting to construct a NotNull object from a null pointer.");
}

void Polly::Details::throwEmptyMaybeException()
{
    throw Error("Attempting to access an empty Maybe object.");
}

void Polly::Details::throwArrayIndexOutOfRangeException(u32 index, u32 n)
{
    throw Error(formatString("Attempting to access List of size {} at index {}.", n, index));
}

void Polly::Details::throwSortedSetEmptyException()
{
    throw Error("Attempting to use an empty SortedSet.");
}
