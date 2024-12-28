/*
This is free and unencumbered software released into the public domain.

Anyone is free to copy, modify, publish, use, compile, sell, or
distribute this software, either in source code form or as a compiled
binary, for any purpose, commercial or non-commercial, and by any
means.

In jurisdictions that recognize copyright laws, the author or authors
of this software dedicate any and all copyright interest in the
software to the public domain. We make this dedication for the benefit
of the public at large and to the detriment of our heirs and
successors. We intend this dedication to be an overt act of
relinquishment in perpetuity of all present and future rights to this
software under copyright law.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.

For more information, please refer to <http://unlicense.org/>
*/

#include "shader.hpp"
#include "archive.hpp"
#include "probe.hpp"
#include "shader_format.hpp"
#include <iostream>
#include <stdexcept>

void extract_shader_pair(Buffer& buffer, const std::string& output_dir_path, const int entry_number)
{
    const uint32_t entry_offset_vert = buffer.read_u32();

    buffer.seek(entry_offset_vert, Buffer::SET);
    const uint32_t size_vert = buffer.read_u32();

    const auto filename_vert = output_dir_path + "/shader" + std::to_string(entry_number) + ".vert";

    FILE* vert = std::fopen(filename_vert.c_str(), "w");
    fwrite(buffer.at(entry_offset_vert + 4), size_vert, 1, vert);
    fclose(vert);

    const uint32_t entry_offset_frag = entry_offset_vert + 4 + size_vert;
    buffer.seek(entry_offset_frag, Buffer::SET);
    const uint32_t size_frag = buffer.read_u32();

    const auto filename_frag = output_dir_path + "/shader" + std::to_string(entry_number) + ".frag";

    FILE* frag = std::fopen(filename_frag.c_str(), "w");
    fwrite(buffer.at(entry_offset_frag + 4), size_frag, 1, frag);
    fclose(frag);
}

void extract_shader_bundle(Buffer& buffer, const std::string& output_dir_path, const int entry_number)
{
    const uint32_t entry_offset = buffer.read_u32();

    buffer.seek(entry_offset, Buffer::SET);
    const uint32_t size = buffer.read_u32();

    const auto filename = output_dir_path + "/shader" + std::to_string(entry_number) + ".bin";
    FILE* out = std::fopen(filename.c_str(), "w");
    fwrite(buffer.at(entry_offset + 4), size, 1, out);
    fclose(out);
}

void extract_shaders(
    const asset_offsets& offsets, Buffer& buffer,
    const std::string& output_dir_path, const archive_version_data& version_data
)
{
    if (offsets.shaders == INVALID_OFFSET) {
        std::cerr << "failed to find shader offsets";
        return;
    }

    int entry_number = 0;
    for (uint32_t i = offsets.shaders; i < offsets.shaders_end; i += 4) {
        buffer.seek(i, Buffer::SET);

        if (version_data.shader == shader_format::PAIR) {
            extract_shader_pair(buffer, output_dir_path, entry_number);
        } else if (version_data.shader == shader_format::BUNDLE) {
            extract_shader_bundle(buffer, output_dir_path, entry_number);
        } else {
            throw std::invalid_argument("extract_shaders: invalid shader_format specified");
        }

        ++entry_number;
    }
    std::cout << "Wrote " << entry_number << " shader entries" << std::endl;
}