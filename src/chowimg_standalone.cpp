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

#include <boost/program_options.hpp>
#include <boost/program_options/options_description.hpp>
#include <cstdio>
#include <exception>
#include <iostream>

#include "stb/stb_image_write.h"
#include "chowimg.hpp"
#include "util.hpp"

namespace po = boost::program_options;

#define PROJECT_NAME "chowimg"

void print_help()
{
    std::cout << "usage: " PROJECT_NAME " input.bin width height output.png" << std::endl;
}

int main(const int argc, char** argv)
{
    po::variables_map opts;

    po::options_description opt_desc;
    opt_desc.add_options()(
        "input",
        "input file containing raw compressed data"
    )(
        "output",
        "where to write the output"
    )(
        "width",
        po::value<int>(),
        "width of the png image that should be created from raw data"
    )(
        "height",
        po::value<int>(),
        "height of the png image that should be created from raw data"
    );

    po::positional_options_description positional_desc;
    positional_desc.add("input", 1).add("width", 1).add("height", 1).add("output", 1);

    try {
        po::store(
            po::command_line_parser(argc, argv)
            .options(opt_desc).positional(positional_desc).run(),
            opts);
    } catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }

    if (opts.count("input") == 0 || opts.count("output") == 0) {
        print_help();
        return 1;
    }

    auto& input_name = opts["input"].as<std::string>();
    auto& output_name = opts["output"].as<std::string>();

    const int width = opts["width"].as<int>();
    const int height = opts["height"].as<int>();
    if (width <= 0 || height <= 0) {
        std::cerr << "invalid image dimensions" << std::endl;
        return 1;
    }

    FILE* input = fopen(input_name.c_str(), "rb");
    if (!input) {
        std::cerr << "failed to open input file" << std::endl;
        return 1;
    }

    Buffer in_buffer(input);
    fclose(input);

    Buffer out_buffer(0xffff);

    int res = chowimg_read(out_buffer, in_buffer, in_buffer.get_size());
    if (res) {
        return 1;
    }

    FILE* out = fopen(output_name.c_str(), "wb");
    fwrite(out_buffer.at(0), out_buffer.tell(), 1, out);
    fclose(out);

    res = stbi_write_png(output_name.c_str(), width, height, 4, out_buffer.at(0), width * 4);
    if (!res) {
        std::cerr << "failed to write image to file " << output_name << std::endl;
        return 1;
    }

    return 0;
}