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

#include "chowimg.hpp"
#include "util.hpp"
#include <boost/container/container_fwd.hpp>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <iostream>

uint32_t read_variable_length_size(Buffer& buffer, uint8_t nibble) {
    uint32_t len = nibble;
    if (nibble == 0xf) {
        uint8_t byte;
        do {
            byte = buffer.read_u8();
            len += byte;
        } while(byte == 0xff);
    }
    return len;
}

int read_hunk(Buffer& out_buffer, Buffer& buffer) {
    uint32_t hunk_compressed_size = buffer.read_u32();
    uint32_t hunk_decompressed_size = 0;
    uint32_t max_offset = buffer.tell() + hunk_compressed_size;
    uint32_t hunk_decompressed_start = out_buffer.tell();
    out_buffer.ensure_writable(hunk_compressed_size);
    while(buffer.tell() < max_offset) {
        uint8_t control_byte = buffer.read_u8();

        uint8_t first_nibble = control_byte >> 4;
        uint8_t second_nibble = control_byte & 0xf;

        uint32_t bytes_to_copy_count = read_variable_length_size(buffer, first_nibble);
        out_buffer.ensure_writable(bytes_to_copy_count);
        out_buffer.write(buffer.at(buffer.tell()), bytes_to_copy_count);

        buffer.seek(bytes_to_copy_count, Buffer::Whence::CURR);
        hunk_decompressed_size += bytes_to_copy_count;

        if (buffer.tell() >= max_offset) {
            break;
        }

        uint16_t rewind_distance = buffer.read_u16();

        uint32_t rewind_start = hunk_decompressed_start + hunk_decompressed_size - rewind_distance;

        if (rewind_distance > hunk_decompressed_size) {
            std::cerr << "read_hunk: rewind distance underflows the hunk (dist=" 
                << rewind_distance << ", hunk_decompressed_size=" << hunk_decompressed_size
                << ")"  << std::endl;
            return 1;
        }

        uint32_t rewind_byte_count = read_variable_length_size(
            buffer, second_nibble) + 4;
        
        out_buffer.ensure_writable(rewind_byte_count);

        // std::cout << "copy from " << rewind_start << " to " << out_buffer.tell() << " x" 
        //     << rewind_byte_count << std::endl; 
    
        out_buffer.copy_from_self(rewind_start, rewind_byte_count);

        hunk_decompressed_size += rewind_byte_count;
    }
    return 0;
}

int chowimg_read(Buffer& out_buffer, Buffer& buffer, uint32_t max_offset) {
    while (buffer.tell() < max_offset) {
        int res = read_hunk(out_buffer, buffer);
        if (res) {
            return 1;
        }
    }
    return 0;
}