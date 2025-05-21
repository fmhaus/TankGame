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
};

struct Rect
{
    f32 x, y, width, height;
};