#pragma once

#include "engine/Types.h"

#include <bit>

namespace MathUtil
{

    const f32 PI_32 = 3.14159265358979323846;
    const f64 PI_64 = 3.14159265358979323846;

    /// Gets the angle in range [-PI, PI]
    static f32 normalize_angle(f32 angle)
    {
        angle = std::fmod(angle + PI_32, 2.0f * PI_32);
        if (angle < 0.0f)
            angle += 2.0f * PI_32;
        return angle - PI_32;
    }

    /// Gets the shortest difference (and direction) of one angle to another
    static f32 normalize_angle_difference(f32 from, f32 to)
    {
        return normalize_angle(to - from);
    }

    /// Divides x by y and returns the smallest integer that is greater or equal the result
    static u32 divide_round_up(u32 x, u32 y)
    {
        if (x == 0)
            return 0;
        return (x + y - 1) / y;
    }

    /// Gets the smallest power of 2 integer that is greater or equal the input value
    static u32 get_bounding_pow2(u32 value)
    {
        return std::bit_ceil(value);
    }

    /// Rotates a vec2 by an angle
    static glm::vec2 rotate(const glm::vec2& vec, f32 angle_rad)
    {
        f32 sin = std::sin(angle_rad);
        f32 cos = std::cos(angle_rad);
        return { vec.x * cos - vec.y * sin, vec.x * sin + vec.y * cos };
    }
}