// Copyright (C) 2025 Cemalettin Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE.

#include "Writer.hpp"

#include "Polly/ToString.hpp"

namespace Polly::ShaderCompiler
{
Writer::Writer()
{
    _buffer.reserve(64);
}

void Writer::appendLine(StringView str)
{
    append(str);
    append("\n");
}

void Writer::append(StringView str)
{
    if (not _buffer.isEmpty() and _buffer.last() == '\n')
    {
        for (int i = 0; i < _indentation; ++i)
        {
            _buffer += "  ";
        }
    }

    _buffer += str;
}

void Writer::indent()
{
    ++_indentation;
}

void Writer::unindent()
{
    assume(_indentation > 0);
    --_indentation;
}

void Writer::openBrace()
{
    appendLine("{");
    indent();
}

void Writer::closeBrace(bool semicolon)
{
    unindent();
    append(semicolon ? "};" : "}");
}

void Writer::clear()
{
    _buffer.clear();
}

void Writer::pad(size_t count)
{
    for (size_t i = 0; i < count; ++i)
    {
        _buffer += ' ';
    }
}

StringView Writer::buffer() const&
{
    return _buffer;
}

String Writer::buffer() &&
{
    return std::move(_buffer);
}

size_t Writer::bufferLength() const
{
    return _buffer.size();
}

int Writer::currentColumn() const
{
    // TODO: optimize this, this is ugly
    auto       column = 0;
    const auto end    = int(_buffer.size()) - 1;

    for (int i = end; i >= 0; --i)
    {
        if (_buffer[i] == '\n')
        {
            break;
        }

        ++column;
    }

    return column;
}

Writer& Writer::operator<<(StringView str)
{
    append(str);
    return *this;
}

Writer& Writer::operator<<(const String& str)
{
    append(str);
    return *this;
}

Writer& Writer::operator<<(const char* str)
{
    append(str);
    return *this;
}

Writer& Writer::operator<<(char ch)
{
    append(StringView{&ch, 1});
    return *this;
}

Writer& Writer::operator<<(int value)
{
    append(toString(value));
    return *this;
}

Writer& Writer::operator<<(unsigned int value)
{
    append(toString(value));
    return *this;
}

Writer& Writer::operator<<(bool value)
{
    append(value ? "true" : "false");
    return *this;
}

Writer& Writer::operator<<(write_newline_tag)
{
    append("\n");
    return *this;
}

Writer& Writer::operator<<(write_newline_lazy_tag)
{
    if (_buffer.isEmpty() or _buffer.last() != '\n')
    {
        append("\n");
    }

    return *this;
}
} // namespace pl::shd
