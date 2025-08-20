import os.path
import sys

TEMPLATE_CPP = """
// clang-format off
#include "%RELATIVE_HPP_FILENAME%"

const Polly::u8 %C_NAME%_data[] = {
%OUTPUT_CPP%
};

const int %C_NAME%_size = sizeof(%C_NAME%_data);
"""

TEMPLATE_HPP = """
// clang-format off
#pragma once

#include <cstdint>
#include <cstddef>
#include "Polly/Span.hpp"
#include "Polly/StringView.hpp"

extern const Polly::u8 %C_NAME%_data[];
extern const int %C_NAME%_size;

static inline auto %C_NAME%_span() -> Polly::Span<Polly::u8> {
    return Polly::Span( reinterpret_cast<const Polly::u8*>( %C_NAME%_data ), %C_NAME%_size );
}

static inline auto %C_NAME%_string_view() -> Polly::StringView {
    return Polly::StringView( reinterpret_cast<const char*>( %C_NAME%_data ), %C_NAME%_size );
}
// clang-format on
"""


class EmbedCommand:
    def __init__(self, filename: str, output_filename_hpp: str, output_filename_cpp: str, c_name: str):
        self.filename = filename
        self.output_filename_hpp = output_filename_hpp
        self.output_filename_cpp = output_filename_cpp
        self.c_name = c_name

    def execute(self):
        source_file = self.filename
        c_name = self.c_name

        with open(source_file, 'rb') as source:
            content = source.read()
            contents_as_hex = [('0x%02x' % b) for b in content]

            content = ""
            for chunk in EmbedCommand.__byte_chunk(contents_as_hex, 16):
                content += ", ".join(chunk)
                content += ",\n"

            output = TEMPLATE_CPP
            output = output.replace(
                "%RELATIVE_HPP_FILENAME%", os.path.basename(self.output_filename_hpp))
            output = output.replace("%OUTPUT_CPP%", content)
            output = output.replace("%C_NAME%", c_name)

            with open(self.output_filename_cpp, "w") as out_fs:
                out_fs.write(output)

            output = TEMPLATE_HPP
            output = output.replace("%C_NAME%", c_name)

            with open(self.output_filename_hpp, "w") as out_fs:
                out_fs.write(output)

    @staticmethod
    def __byte_chunk(seq, chunk_size):
        for i in range(0, len(seq), chunk_size):
            yield seq[i: i + chunk_size]
