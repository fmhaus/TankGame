#pragma once

#include <stdint.h>
#include <cassert>
#include <cmath>
#include <algorithm>
#include <glm/glm.hpp>

typedef uint8_t u8;
typedef int8_t s8;
typedef uint16_t u16;
typedef int16_t s16;
typedef uint32_t u32;
typedef int32_t s32;
typedef uint64_t u64;
typedef int64_t s64;
typedef size_t usz;
typedef float f32;
typedef double f64;

/// Base to disable all copying of the derived class
struct NoCopy
{
    NoCopy() = default;
    ~NoCopy() = default;

    NoCopy(const NoCopy&) = delete;
    NoCopy& operator=(const NoCopy&) = delete;

    NoCopy(NoCopy&&) noexcept = default;
    NoCopy& operator=(NoCopy&&) noexcept = default;
};

/// This sets the id to 0 on move, prevents double delete when using moving
struct GLResource
{
    u32 id;

    u32* operator&() { return &id; }
    operator u32() const { return id; }

    GLResource() = default;
    GLResource(GLResource&& other) noexcept
        : id(other.id)
    {
        other.id = 0;
    }

    GLResource& operator=(GLResource&& other) noexcept
    {
        if (&id != &other)
        {
            id = other.id;
            other.id = 0;
        }
        return *this;
    }

    GLResource& operator=(u32 id) noexcept
    {
        this->id = id;
        return *this;
    }
};


struct Color
{
    f32 r, g, b, a;

    glm::vec4 to_vec() const
    {
        return glm::vec4(r, g, b, a);
    }

	static Color Red() { return { 1.0f, 0.0f, 0.0f, 1.0f }; }
	static Color Green() { return { 0.0f, 1.0f, 0.0f, 1.0f }; }
	static Color Blue() { return { 0.0f, 0.0f, 1.0f, 1.0f }; }
	static Color Black() { return { 0.0f, 0.0f, 0.0f, 1.0f }; }
	static Color White() { return { 1.0f, 1.0f, 1.0f, 1.0f }; }
	static Color Transparent() { return { 0.0f, 0.0f, 0.0f, 0.0f }; }

    static Color from_hex(u32 hex)
    {
        return {
            ((hex >> 16) & 0xFF) / 255.0f,
            ((hex >> 8) & 0xFF) / 255.0f,
            (hex & 0xFF) / 255.0f,
            ((hex >> 24) & 0xFF) / 255.0f
        };
	}
};

struct Rect
{
    f32 x, y, width, height;
};

template<typename T, usz N>
using Array = std::array<T, N>;

template<typename T, usz N, usz M>
using Array2D = std::array<std::array<T, M>, N>;