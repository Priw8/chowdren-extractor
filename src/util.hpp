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

#include <cstdint>
#include <cstring>
#include <stdexcept>

uint16_t read_little_endian_u16(const uint8_t* const data);

uint32_t read_little_endian_u32(const uint8_t* const data);

float read_little_endian_f32(const uint8_t* const data);

void write_little_endian_u16(uint8_t* data, uint16_t val);

void write_little_endian_u32(uint8_t* data, uint32_t val);

void write_little_endian_f32(uint8_t* data, float val);

class Buffer {
    uint32_t size;
    uint32_t offset = 0;
    uint8_t* buffer;

    inline void bounds_check(uint32_t required_size) {
        if (this->offset + required_size > this->size) {
            throw std::range_error("Buffer::bounds_check: attempt to write or read past the buffer size");
        }
    }
public:
    enum Whence {
        SET,
        CURR,
        END
    };

    Buffer(uint32_t size);
    Buffer(FILE* file);
    Buffer(Buffer&&) = delete;
    ~Buffer();

    void seek(uint32_t offset, Whence whence);
    inline uint32_t tell() const { return this->offset; } ;

    inline uint32_t get_size() const { return this->size; };

    void reserve(uint32_t size);
    void reserve(uint32_t size, uint32_t extra_alloc);

    inline void ensure_writable(uint32_t size) {
        reserve(this->size + size);
    }
    inline void ensure_writable(uint32_t size, uint32_t extra_alloc) {
        reserve(this->size + size, extra_alloc);
    }

    inline uint8_t* at(uint32_t offset) const { return &this->buffer[offset]; };

    inline uint8_t read_u8() { 
        bounds_check(1);
        return this->buffer[this->offset++]; 
    };
    inline uint16_t read_u16()  { 
        bounds_check(2);
        uint16_t res = read_little_endian_u16(this->buffer + this->offset); 
        this->offset += 2;
        return res;
    };
    inline uint32_t read_u32() {
        bounds_check(4);
        uint32_t res = read_little_endian_u32(this->buffer + this->offset); 
        this->offset += 4;
        return res;
    };

    inline void write_u8(uint8_t val) {
        bounds_check(1);
        this->buffer[this->offset] = val;
        ++this->offset;
    }

    inline void write_u16(uint16_t val) {
        bounds_check(2);
        write_little_endian_u16(this->buffer + this->offset, val);
        this->offset += 2;
    }

    inline void write_u32(uint32_t val) {
        bounds_check(4);
        write_little_endian_u32(this->buffer + this->offset, val);
        this->offset += 4;
    }

    inline void write_f32(float val) {
        bounds_check(4);
        write_little_endian_f32(this->buffer + this->offset, val);
        this->offset += 4;
    }

    inline void write(uint8_t* source, uint32_t count) {
        bounds_check(count);
        memcpy(this->buffer + offset, source, count);
        this->offset += count;
    }

    inline void write(Buffer& source, uint32_t count) {
        write(source.buffer, count);
    }

    void copy_from_self(uint32_t from, uint32_t count);
};
