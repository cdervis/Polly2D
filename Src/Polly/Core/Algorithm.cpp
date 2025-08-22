// Copyright (C) 2025 Cemalettin Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE.

#include "Polly/Algorithm.hpp"

#include "Polly/List.hpp"
#include "Polly/Math.hpp"
#include "Polly/StringView.hpp"

int Polly::levensteinDistance(StringView s1, StringView s2)
{
    const auto s1Len = s1.size();
    const auto s2Len = s2.size();

    auto distances = List<int, 32>(s2Len + 1);
    Polly::fillByIncrementing(distances, 0);

    for (auto i = 0u; i < s1Len; ++i)
    {
        int previousDistance = 0;

        for (auto j = 0u; j < s2Len; ++j)
        {
            distances[j + 1] =
                min(std::exchange(previousDistance, distances[j + 1]) + (s1[i] == s2[j] ? 0 : 1),
                    distances[j] + 1,
                    distances[j + 1] + 1);
        }
    }

    return distances[s2Len];
}
