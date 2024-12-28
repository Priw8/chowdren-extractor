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

#include "archive.hpp"
#include "shader_format.hpp"
#include <iostream>
#include <stdexcept>

archive_version get_archive_version(const std::string& name)
{
    if (name == "v0") {
        return archive_version::V0;
    } else if (name == "v0-ctr-chow") {
        return archive_version::V0_CTR_CHOW;
    } else if (name == "v0-ctr-zlib") {
        return archive_version::V0_CTR_ZLIB;
    } else if (name == "v1") {
        return archive_version::V1;
    } else if (name == "v2") {
        return archive_version::V2;
    } else if (name == "v3") {
        return archive_version::V3;
    } else {
        std::cerr << "get_archive_version: specified format '" << name << "' is invalid";
        return archive_version::INVALID;
    }
}

const archive_version_data& get_archive_version_data(const archive_version version)
{
    // The Escapists, Freedom Planet (2015 version), Not a Hero
    static constexpr archive_version_data data_v0 = {
        image_entry_format::V0,
        image_container_format::ZLIB,
        sound_entry_format::V1,
        shader_format::PAIR,
        false
    };

    // Dodge Club Pocket
    static constexpr archive_version_data data_v0_ctr_chow = {
        image_entry_format::V1,
        image_container_format::CHOWIMG,
        sound_entry_format::V0,
        shader_format::BUNDLE,
        false
    };

    // Noitu Love: Devolution
    static constexpr archive_version_data data_v0_ctr_zlib = {
        image_entry_format::V1,
        image_container_format::ZLIB,
        sound_entry_format::V0,
        shader_format::BUNDLE,
        false
    };

    // Iconoclasts
    static constexpr archive_version_data data_v1 = {
        image_entry_format::V2,
        image_container_format::ZLIB,
        sound_entry_format::V1,
        shader_format::PAIR,
        true
    };

    // Baba is You
    static constexpr archive_version_data data_v2 = {
        image_entry_format::V1,
        image_container_format::CHOWIMG,
        sound_entry_format::V1,
        shader_format::PAIR,
        true
    };

    // Petal Crash, Cyber Shadow, Mighty Goose
    static constexpr archive_version_data data_v3 = {
        image_entry_format::V3,
        image_container_format::ZLIB,
        sound_entry_format::V2,
        shader_format::PAIR,
        true
    };

    switch (version) {
        case archive_version::V0:
            return data_v0;
        case archive_version::V0_CTR_CHOW:
            return data_v0_ctr_chow;
        case archive_version::V0_CTR_ZLIB:
            return data_v0_ctr_zlib;
        case archive_version::V1:
            return data_v1;
        case archive_version::V2:
            return data_v2;
        case archive_version::V3:
            return data_v3;
        default:
            throw std::invalid_argument("get_archive_version_data: invalid archive_version");
    }
}