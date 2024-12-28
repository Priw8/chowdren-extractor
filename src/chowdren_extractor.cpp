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

#include <boost/filesystem/file_status.hpp>
#include <boost/program_options.hpp>
#include <boost/program_options/errors.hpp>
#include <boost/program_options/options_description.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/program_options/positional_options.hpp>
#include <boost/program_options/value_semantic.hpp>
#include <boost/program_options/variables_map.hpp>
#include <boost/filesystem.hpp>
#include <cstdlib>
#include <exception>
#include <ios>
#include <iostream>
#include <cstdio>
#include <ostream>
#include <string>
#include <zlib.h>

#include "archive.hpp"
#include "image.hpp"
#include "probe.hpp"
#include "shader.hpp"
#include "sound.hpp"
#include "stb/stb_image_write.h"
#include "util.hpp"

#define PROJECT_NAME "chowdren-extractor"

namespace po = boost::program_options;
namespace fs = boost::filesystem;


int parse_args(po::variables_map& opts, int argc, char** argv)
{
    po::options_description optdesc_named("Named options");
    optdesc_named.add_options()
    (
        "format",
        po::value<std::string>(),
        "see versions.md for details"
    )
    (
        "probe-offsets",
        "only find offsets and exit"
    )
    (
        "no-images",
        "skip extracting images"
    )
    (
        "no-audio",
        "skip extracting audio"
    )
    (
        "no-shaders",
        "skip extracting shaders"
    )
    (
        "help",
        "print help message"
    );
    po::options_description optdesc_positional("Positional options");
    optdesc_positional.add_options()
    (
        "input",
        "input file"
    )
    (
        "output",
        "output directory"
    );

    po::options_description optdesc("Available options");
    optdesc.add(optdesc_named).add(optdesc_positional);

    po::positional_options_description p;
    p.add("input", 1).add("output", 1);

    try {
        po::store(
            po::command_line_parser(argc, argv)
            .options(optdesc).positional(p).run(), opts);

        po::notify(opts);
    } catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }

    if (!opts.count("input") || !opts.count("output") || opts.count("help")) {
        std::cout << "Usage: " PROJECT_NAME " [options] input.dat output-dir" << std::endl;
        optdesc_named.print(std::cout);
        return 1;
    }

    if (opts["format"].empty()) {
        std::cout << "Specifying --format is required. See versions.md for more details." << std::endl;
        optdesc_named.print(std::cout);
        return 1;
    }

    return 0;
}


int main(const int argc, char** argv)
{
    po::variables_map opts;
    if (parse_args(opts, argc, argv)) {
        return 1;
    }

    auto& input_file_path = opts["input"].as<std::string>();
    auto& output_dir_path = opts["output"].as<std::string>();

    if (!fs::is_regular_file(input_file_path)) {
        std::cerr << input_file_path << ": not a regular file" << std::endl;
        return 1;
    }
    if (!fs::is_directory(output_dir_path)) {
        std::cerr << output_dir_path << ": directory does not exist" << std::endl;
        return 1;
    }

    FILE* file = std::fopen(input_file_path.c_str(), "rb");

    Buffer input_buffer(file);

    std::fclose(file);

    const archive_version version = get_archive_version(opts["format"].as<std::string>());
    const archive_version_data& version_data = get_archive_version_data(version);

    asset_offsets offsets{};
    if (find_asset_offsets(offsets, input_buffer, version_data)) {
        std::cerr << "failed to find asset_offsets" << std::endl;
        return 1;
    }

    std::cout
        << "Determined following offsets:" << std::endl << std::hex;
    if (offsets.images != INVALID_OFFSET)
        std::cout << "  - images:     0x" << offsets.images << " - 0x" << offsets.images_end
            << std::endl;

    if (offsets.sounds != INVALID_OFFSET)
        std::cout << "  - sounds:     0x" << offsets.sounds << " - 0x" << offsets.sounds_end
            << std::endl;

    if (offsets.fonts != INVALID_OFFSET)
        std::cout << "  - fonts:      0x" << offsets.fonts << " - 0x" << offsets.fonts_end
            << std::endl;

    if (offsets.shaders != INVALID_OFFSET)
        std::cout << "  - shaders:    0x" << offsets.shaders << " - 0x" << offsets.shaders_end
            << std::endl;

    if (offsets.files != INVALID_OFFSET)
        std::cout << "  - files:      0x" << offsets.files << " - 0x" << offsets.files_end
            << std::endl;

    if (offsets.platform != INVALID_OFFSET)
        std::cout << "  - platform:   0x" << offsets.platform << " - 0x" << offsets.platform_end
            << std::endl;

    std::cout << "  - type_sizes: 0x" << offsets.sizes << std::endl << std::dec;

    if (opts.count("probe-offsets")) {
        return 0;
    }

    if (!opts.count("no-images")) {
        extract_images(offsets, input_buffer, output_dir_path, version_data);
    }

    if (!opts.count("no-audio")) {
        const sound_entry_format format = version_data.sound_entry;
        extract_audio(offsets, input_buffer, output_dir_path, format);
    }

    if (!opts.count("no-shaders")) {
        extract_shaders(offsets, input_buffer, output_dir_path, version_data);
    }

    return 0;
}