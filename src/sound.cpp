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

#include "sound.hpp"
#include "archive.hpp"
#include "probe.hpp"
#include <iostream>
#include <stdexcept>
#include <string>

sound_entry_format get_sound_format(const std::string& name) {
    if (name == "v0") {
        return sound_entry_format::V0;    
    } else if (name == "v1") {
        return sound_entry_format::V1;
    } else if (name == "v2") {
        return sound_entry_format::V2;
    } else {
        return sound_entry_format::INVALID;
    }
}

const sound_offsets& get_sound_offsets(sound_entry_format format) {
    static const sound_offsets offsets_v0 =  {8, 16};
    static const sound_offsets offsets_v1 =  {4, 8};
    static const sound_offsets offsets_v2 =  {16, 20};

    if (format == sound_entry_format::V0)  return offsets_v0;
    if (format == sound_entry_format::V1)  return offsets_v1;
    if (format == sound_entry_format::V2)   return offsets_v2;

    throw std::invalid_argument("get_sound_offsets: received invalid sound format");
}


const std::string& get_sound_container_extension(uint32_t sound_container_format_id) {
    static const std::string extension_ogg  = ".ogg";
    static const std::string extension_wav  = ".wav";
    static const std::string extension_cwav = ".cwav";
    static const std::string extension_invalid = ".bin";

    switch(sound_container_format_id) {
        case 1:
            return extension_wav;
        case 2:
            return extension_ogg;
        case 3:
            return extension_cwav;
        default:
            std::cerr << "warning: unknown sound container format identifier: "
               << sound_container_format_id << std::endl;
            return extension_invalid;
    }
}

void extract_audio(
  asset_offsets& offsets, Buffer& buffer, 
  const std::string& output_dir_path, sound_entry_format format
) {
    if (offsets.sounds == INVALID_OFFSET) {
        std::cerr << "failed to find sound offsets";
        return;
    }

    const sound_offsets& sound_offsets = get_sound_offsets(format);

    int entry_number = 0;
    for (uint32_t i=offsets.sounds; i<offsets.sounds_end; i+=4) {
        buffer.seek(i, Buffer::SET);
        uint32_t entry_offset = buffer.read_u32();

        buffer.seek(entry_offset, Buffer::SET);
        uint32_t container_type = buffer.read_u32();

        buffer.seek(entry_offset + sound_offsets.size, Buffer::SET);
        uint32_t size = buffer.read_u32();

        auto& extension = get_sound_container_extension(container_type);
        auto filename = output_dir_path + "/audio" + std::to_string(entry_number) + extension;
    
        FILE* out = std::fopen(filename.c_str(), "wb");
        fwrite(buffer.at(entry_offset + sound_offsets.data), size, 1, out);
        fclose(out);
        ++entry_number;
    }
    std::cout << "Wrote " << entry_number << " audio files" << std::endl;
}
