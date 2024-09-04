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

#include "image.hpp"
#include "image_format.hpp"
#include "probe.hpp"
#include "stb/stb_image_write.h"
#include "chowimg.hpp"
#include "util.hpp"
#include <iostream>
#include <zlib.h>

void parse_image_entry(
    parsed_image_entry& entry, Buffer& buffer, const archive_version_data& version_data
) {
    if (version_data.image_entry == image_entry_format::V0) {
        entry.width = buffer.read_u16();
        entry.height = buffer.read_u16();
        buffer.seek(8, Buffer::CURR); // Skip u16, u16, u8[4] unknown values
        entry.size = buffer.read_u32();
        entry.data_offset = buffer.tell();
    } else if (version_data.image_entry == image_entry_format::V1) {
        entry.width = buffer.read_u16();
        entry.height = buffer.read_u16();
        buffer.seek(9, Buffer::CURR); // Skip u16, u16, u8[5] unknown values
        entry.size = buffer.read_u32();
        entry.data_offset = buffer.tell();
    } else if (version_data.image_entry == image_entry_format::V2) {
        entry.width = buffer.read_u16();
        entry.height = buffer.read_u16();
        buffer.seek(4, Buffer::CURR); // Skip u16, u16 unknown values
        uint8_t ex_count = buffer.read_u8();
        buffer.seek(ex_count * 4, Buffer::CURR); // Skip extra dimension data
        entry.size = buffer.read_u32();
        entry.data_offset = buffer.tell();
    } else if (version_data.image_entry == image_entry_format::V3) {
        entry.width = buffer.read_u16();
        entry.height = buffer.read_u16();
        buffer.seek(4 + 4, Buffer::CURR); // Skip float, float
        uint8_t ex_count = buffer.read_u8();
        buffer.seek(ex_count * 8, Buffer::CURR); // Skip extra dimension data
        entry.size = buffer.read_u32();
        entry.data_offset = buffer.tell();
    } else {
        throw std::invalid_argument("parse_image_entry: invalid image entry type");
    }
}

void extract_images(
    asset_offsets& offsets, Buffer& buffer, 
    const std::string& output_dir_path, const archive_version_data& version_data
) {
    if (offsets.images == INVALID_OFFSET) {
        std::cerr << "failed to find image offsets";
        return;
    }
    
    // I don't know the size of the compressed data, so let's just get a 16mb buffer that should be large enough for everything
    constexpr const int buffer_size = 0x1000000;
    Buffer temp_buffer(buffer_size);

    int entry_number = 0;
    int extracted_number = 0;
    for (uint32_t i=offsets.images; i<offsets.images_end; i+=4) {
        buffer.seek(i, Buffer::SET);
        uint32_t entry_offset = buffer.read_u32();
        buffer.seek(entry_offset, Buffer::SET);

        parsed_image_entry entry;
        parse_image_entry(entry, buffer, version_data);
        uint8_t* image_data = buffer.at(entry.data_offset);

        image_container_format format = version_data.image;
        unsigned long out_size = buffer_size;
        if (format == image_container_format::ZLIB || format == image_container_format::CHOWIMG) {
            bool decompression_success = false;
            if (format == image_container_format::ZLIB) {
                int result = uncompress(temp_buffer.at(0), &out_size, image_data, entry.size);
                if (result != Z_OK) {
                    std::cerr << "zlib decompression failure for image" << entry_number 
                      << " (" << entry.width << "x" << entry.height << "), image_offset=0x"
                      << std::hex << entry.data_offset << ", entry_offset=0x" 
                      << entry_offset << std::dec << std::endl;
                } else {
                    decompression_success = true;
                }
            } else if (format == image_container_format::CHOWIMG) {
                temp_buffer.seek(0, Buffer::SET);
                buffer.seek(entry.data_offset, Buffer::SET);
                int result = chowimg_read(temp_buffer, buffer, 
                    entry.data_offset + entry.size);

                if (result) {
                    std::cerr << "chowimg decompression failure for image" << entry_number 
                      << " (" << entry.width << "x" << entry.height << "), image_offset=0x"
                      << std::hex << entry.data_offset << ", entry_offset=0x" 
                      << entry_offset << std::dec << std::endl;
                } else {
                    decompression_success = true;
                }
            }
            if (decompression_success) {
                auto filename = output_dir_path + "/image" + std::to_string(entry_number) + ".png";
                stbi_write_png(filename.c_str(), entry.width, entry.height, 4, 
                    temp_buffer.at(0), entry.width * 4);
                ++extracted_number;
            }
        } else if (format == image_container_format::RAW) {
            auto filename = output_dir_path + "/image" + std::to_string(entry_number) + "-" 
              + std::to_string(entry.width) + "x" + std::to_string(entry.height) + ".bin";
            FILE* out = fopen(filename.c_str(), "wb");
            fwrite(image_data, entry.size, 1, out);
            fclose(out);
            ++extracted_number;
        }
        // std::cout << "Processing image entry #" << entry_number << std::endl;
        ++entry_number;
    };
    std::cout << "Wrote " << extracted_number << " images" << std::endl;
}