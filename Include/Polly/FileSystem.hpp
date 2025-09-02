// Copyright (C) 2025 Cem Dervis
// This file is part of Polly, a 2D C++ game framework for minimalists.
// For conditions of distribution and use, see copyright notice in LICENSE, or https://polly2d.org.

#pragma once

#include "Polly/ByteBlob.hpp"
#include "Polly/List.hpp"
#include "Polly/String.hpp"

namespace Polly::FileSystem
{
/// Loads the entire data of an archived asset unconditionally.
Maybe<ByteBlob> loadAssetData(StringView filename);

Maybe<String> loadTextFileFromDisk(StringView filename);

[[nodiscard]]
Maybe<ByteBlob> loadFileFromDisk(StringView filename);

void writeBinaryFileToDisk(StringView filename, Span<u8> contents);

void writeTextFileToDisk(StringView filename, StringView contents);

[[nodiscard]]
String fileSizeDisplayString(uintmax_t size);

[[nodiscard]]
String cleanPath(StringView path, Maybe<bool> withEndingSlash = none);

void transformToCleanPath(String& path, Maybe<bool> withEndingSlash = none);

[[nodiscard]]
String pathCombine(StringView path1, StringView path2);

[[nodiscard]]
String pathCombine(Span<StringView> paths);

[[nodiscard]]
String pathFilename(StringView path, bool withExtension = true);

[[nodiscard]]
String pathExtension(StringView path, bool withDot = true);

[[nodiscard]]
String pathRelative(StringView from, StringView to);

[[nodiscard]]
bool pathExists(StringView path);

[[nodiscard]]
String pathParent(StringView path);

void createDirectories(StringView path);

[[nodiscard]]
String pathReplaceExtension(StringView path, StringView newExtension);

[[nodiscard]]
Maybe<String> randomWritablePath(StringView companyName, StringView gameName);
} // namespace Polly::FileSystem
