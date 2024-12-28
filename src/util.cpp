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

#include "util.hpp"
#include <cstdio>
#include <stdexcept>
#include <cstdlib>
#include <cstdint>

uint16_t read_little_endian_u16(const uint8_t* const data)
{
    return *data | (*(data + 1) << 8);
}

uint32_t read_little_endian_u32(const uint8_t* const data)
{
    return *data | (*(data + 1) << 8) | (*(data + 2) << 16) | (*(data + 3) << 24);
}

float read_little_endian_f32(const uint8_t* const data)
{
    uint32_t tmp = (*data | (*(data + 1) << 8) | (*(data + 2) << 16) | (*(data + 3) << 24));
    return *(float*)(&tmp);
}

void write_little_endian_u16(uint8_t* data, const uint16_t val)
{
    *data = val & 0xff;
    *(data + 1) = val >> 8;
}

void write_little_endian_u32(uint8_t* data, const uint32_t val)
{
    *data = val & 0xff;
    *(data + 1) = (val >> 8) & 0xff;
    *(data + 2) = (val >> 16) & 0xff;
    *(data + 3) = (val >> 24) & 0xff;
}

void write_little_endian_f32(uint8_t* data, float val)
{
    const uint32_t tmp = *(uint32_t*)(&val);
    *data = tmp & 0xff;
    *(data + 1) = (tmp >> 8) & 0xff;
    *(data + 2) = (tmp >> 16) & 0xff;
    *(data + 3) = (tmp >> 24) & 0xff;
}

Buffer::Buffer(const uint32_t size)
{
    this->size = size;
    this->buffer = static_cast<uint8_t*>(malloc(size));
}

Buffer::Buffer(FILE* file)
{
    const long prev_seek = ftell(file);
    fseek(file, 0, SEEK_END);
    this->size = ftell(file);
    this->buffer = static_cast<uint8_t*>(malloc(this->size));
    fseek(file, 0, SEEK_SET);
    fread(this->buffer, this->size, 1, file);
    fseek(file, prev_seek, SEEK_SET);
}

Buffer::~Buffer()
{
    free(this->buffer);
}

void Buffer::seek(const uint32_t offset, const Buffer::Whence whence)
{
    switch (whence) {
        case Buffer::Whence::SET:
            this->offset = offset;
            break;
        case Buffer::Whence::END:
            this->offset = this->size - 1 - offset;
            break;
        case Buffer::Whence::CURR:
            this->offset += offset;
            break;
        default:
            throw std::invalid_argument("Buffer::seek: invalid whence");
    }
}

void Buffer::reserve(const uint32_t size)
{
    reserve(size, 0);
}

void Buffer::reserve(const uint32_t size, const uint32_t extra_alloc)
{
    if (this->size < size) {
        this->buffer = static_cast<uint8_t*>(realloc(buffer, size + extra_alloc));
        this->size = size;
    }
}

// Made for cases where destination and source overlap and memcpy can't be used
// interestingly though, we also don't want the behavior of memmove.
// We need a very specific behaviour for overlapping regions, and it's
// achieved by copying byte-by-byte like below.
void Buffer::copy_from_self(const uint32_t from, const uint32_t count)
{
    bounds_check(count);
    const uint32_t max = from + count;
    for (uint32_t i = from; i < max; ++i) {
        this->buffer[this->offset] = this->buffer[i];
        ++this->offset;
    }
}