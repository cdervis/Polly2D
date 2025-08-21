// Copyright (C) 2025 Cemalettin Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE.

#pragma once

#include "Polly/ByteBlob.hpp"
#include "Polly/List.hpp"
#include "Polly/String.hpp"

namespace Polly::FileSystem
{
/// Loads the entire data of an archived asset unconditionally.
Maybe<ByteBlob> loadAssetData(StringView filename);

Maybe<String> loadTextFileFromDisk(StringView filename);

/// Loads a file from disk.
[[nodiscard]]
Maybe<ByteBlob> loadFileFromDisk(StringView filename);

/// Writes a file to disk.
void writeFileToDisk(StringView filename, Span<u8> contents);

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
Maybe<String> randomWritablePath(StringView companyName, StringView appName);
} // namespace Polly::FileSystem
