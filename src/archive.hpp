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

#pragma once

#include "image_format.hpp"
#include "shader_format.hpp"
#include "sound_format.hpp"
#include <cstdint>
#include <string>

struct asset_offsets {
    uint32_t images;
    uint32_t sounds;
    uint32_t fonts;
    uint32_t shaders;
    uint32_t files;
    uint32_t platform;

    uint32_t images_end;
    uint32_t sounds_end;
    uint32_t fonts_end;
    uint32_t shaders_end;
    uint32_t files_end;
    uint32_t platform_end;

    uint32_t sizes;
};

enum class archive_version {
    INVALID,
    V0, V0_CTR_CHOW, V0_CTR_ZLIB,
    V1,
    V2,
    V3
};

struct archive_version_data {
    image_entry_format image_entry;
    image_container_format image;
    sound_entry_format sound_entry;
    shader_format shader;
    bool has_platform_data;
};

archive_version get_archive_version(const std::string& name);

const archive_version_data& get_archive_version_data(archive_version version);