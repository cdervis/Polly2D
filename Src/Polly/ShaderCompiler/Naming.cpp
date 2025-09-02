// Copyright (C) 2025 Cem Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE, or https://polly2d.org.

#include "Naming.hpp"

#include "Polly/String.hpp"

namespace Polly::ShaderCompiler
{
bool Naming::isIdentifierForbidden(StringView identifier)
{
    return identifier.startsWith(forbiddenIdentifierPrefix);
}
} // namespace Polly::ShaderCompiler
