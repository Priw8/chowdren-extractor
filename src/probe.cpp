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

#include "probe.hpp"
#include "archive.hpp"
#include "util.hpp"
#include <iostream>
#include <vector>

bool is_valid_offset(uint8_t* mmap, uint32_t file_size, uint32_t offset) {
    return offset < file_size && read_little_endian_u32(mmap + offset) < file_size;
}

offset_range try_offset(
    uint8_t* mmap, uint32_t file_size, uint32_t offset, const archive_version_data& version_data
) {
    if (!is_valid_offset(mmap, file_size, offset)) return std::nullopt;

    uint32_t type_sizes_size = version_data.has_platform_data ? 24 : 20;
    uint32_t first_offset = offset;
    uint32_t first_entry = read_little_endian_u32(mmap + first_offset);
    for (uint32_t i=first_offset+4; i<first_entry; i+=4) {
        uint32_t entry = read_little_endian_u32(mmap + i);
        if (!is_valid_offset(mmap, file_size, i) || entry < first_entry) {
            uint32_t distance_from_first_entry = first_entry - i;
            if (distance_from_first_entry <= type_sizes_size) {
                // i is likely somewhere in type_sizes, so we just passed the last offset
                uint32_t last_offset = first_entry - type_sizes_size - 4;
                return std::make_pair(first_offset, last_offset);
            } else {
                return std::nullopt;
            }
        }
    }
    return std::nullopt;
}

offset_range find_offset_range(
  uint8_t* mmap, uint32_t file_size, const archive_version_data& version_data
) {
    // Unfortunately the offsets are not always aligned to 4 bytes
    for (uint32_t i=0; i<file_size; ++i) {
        offset_range ranges = try_offset(mmap, file_size, i, version_data);
        if (ranges.has_value()) {
            return ranges;
        }
    }
    return std::nullopt;
}


uint32_t find_u32(uint8_t* mmap, uint32_t val, uint32_t file_size, uint32_t alignment) {
    for (uint32_t i=alignment % 4; i<file_size; i+=4) {
        if (read_little_endian_u32(mmap + i) == val) return i;
    }
    return INVALID_OFFSET;
}



uint32_t find_type_sizes(
    uint8_t* mmap, uint32_t file_size, const archive_version_data& version_data
) {
    offset_range range = find_offset_range(mmap, file_size, version_data);
    std::cout << "Determined offset range [0x" << std::hex << range->first 
        << ", 0x" << range->second << "]" << std::dec << std::endl;
    if (!range.has_value()) {
        return INVALID_OFFSET;
    }

    return range->second + 4;
}

uint32_t first_valid_offset(std::vector<uint32_t>& offsets, uint32_t initial_index) {
    for (uint32_t i=initial_index; i<offsets.size(); ++i) {
        if (offsets[i] != INVALID_OFFSET) {
            return offsets[i];
        }
    }
    return INVALID_OFFSET;
}

uint32_t find_asset_offsets(
    asset_offsets& offsets, Buffer& buffer, const archive_version_data& version_data
) {
    // For these operations in particular, I think working with
    // the raw uint8_t* makes things more convenient.
    uint8_t* mmap = buffer.at(0);
    uint32_t file_size = buffer.get_size();

    uint32_t type_sizes_offset = find_type_sizes(mmap, file_size, version_data);
    if (type_sizes_offset == INVALID_OFFSET) {
        // std::cerr << "Warning: failed to locate type_sizes using the primary method, " 
        //     "potentially incorrect fallback will be used!" << std::endl;

        // // It's possible that there were no shaders, or that they aren't in plaintext.
        // type_sizes_offset = find_type_sizes_fallback_method(mmap, file_size);
        // if (type_sizes_offset == INVALID_OFFSET) {
        //     return 1;
        // }
        return 1;
    }
    
    uint32_t size_images    = read_little_endian_u32(mmap + type_sizes_offset);
    uint32_t size_sounds    = read_little_endian_u32(mmap + type_sizes_offset + 4);
    uint32_t size_fonts     = read_little_endian_u32(mmap + type_sizes_offset + 8);
    uint32_t size_shaders   = read_little_endian_u32(mmap + type_sizes_offset + 12);
    uint32_t size_files     = read_little_endian_u32(mmap + type_sizes_offset + 16);
    uint32_t size_platform  = version_data.has_platform_data
        ? read_little_endian_u32(mmap + type_sizes_offset + 20)
        : 0;

    std::cout << "type_sizes = {0x" << std::hex << size_images << ", 0x" << size_sounds <<
        ", 0x" << size_fonts << ", 0x" << size_shaders << ", 0x" << size_platform << ", 0x" 
        << size_platform << "}" << std::dec << std::endl;

    // Sanity check
    if (file_size < size_images + size_sounds + size_fonts 
      + size_shaders + size_files + size_platform) {
        std::cerr << "find_asset_offsets: sanity check failed" << std::endl;
        return 1;
    }

    uint32_t data_platform = file_size - size_platform;
    uint32_t data_files    = data_platform - size_files;
    uint32_t data_shaders  = data_files - size_shaders;
    uint32_t data_fonts    = data_shaders - size_fonts;
    uint32_t data_sounds   = data_fonts - size_sounds;
    uint32_t data_images   = data_sounds - size_images;

    uint32_t max_search_offset = type_sizes_offset;

    uint32_t alignment = type_sizes_offset % 4;

    offsets.images = find_u32(mmap, data_images, max_search_offset, alignment);
    offsets.sounds = find_u32(mmap, data_sounds, max_search_offset, alignment);
    offsets.fonts = find_u32(mmap, data_fonts, max_search_offset, alignment);
    offsets.shaders = find_u32(mmap, data_shaders, max_search_offset, alignment);
    offsets.files = find_u32(mmap, data_files, max_search_offset, alignment);
    offsets.platform = find_u32(mmap, data_platform, max_search_offset, alignment);
    offsets.sizes = type_sizes_offset;

    std::vector<uint32_t> all_offsets = {
        offsets.images, offsets.sounds, offsets.fonts, 
        offsets.shaders, offsets.files, offsets.platform, offsets.sizes
    };

    offsets.images_end    = first_valid_offset(all_offsets, 1);
    offsets.sounds_end    = first_valid_offset(all_offsets, 2);
    offsets.fonts_end     = first_valid_offset(all_offsets, 3);
    offsets.shaders_end   = first_valid_offset(all_offsets, 4);
    offsets.files_end     = first_valid_offset(all_offsets, 5);
    offsets.platform_end  = first_valid_offset(all_offsets, 6);

    return 0;
}