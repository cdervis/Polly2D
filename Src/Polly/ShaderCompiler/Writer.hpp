// Copyright (C) 2025 Cemalettin Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE.

#pragma once

#include "Polly/String.hpp"

namespace Polly::ShaderCompiler
{
enum write_newline_tag
{
    wnewline,
};

enum write_newline_lazy_tag
{
    wnewline_lazy,
};

class Writer final
{
  public:
    Writer();

    void appendLine(StringView str);

    void append(StringView str);

    void indent();

    void unindent();

    void openBrace();

    void closeBrace(bool semicolon = false);

    void clear();

    Writer& operator<<(StringView str);

    Writer& operator<<(const String& str);

    Writer& operator<<(const char* str);

    Writer& operator<<(char ch);

    Writer& operator<<(int value);

    Writer& operator<<(unsigned int value);

    Writer& operator<<(bool value);

    Writer& operator<<(write_newline_tag);

    Writer& operator<<(write_newline_lazy_tag);

    Writer& operator<<(float) = delete;

    Writer& operator<<(double) = delete;

    void pad(size_t count);

    StringView buffer() const&;

    String buffer() &&;

    size_t bufferLength() const;

    int currentColumn() const;

  private:
    String _buffer;
    int    _indentation = 0;
};
} // namespace Polly::ShaderCompiler
