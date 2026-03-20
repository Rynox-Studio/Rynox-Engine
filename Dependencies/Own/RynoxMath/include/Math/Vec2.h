#pragma once

#include <cstddef>

namespace Rynox::Math
{
    struct Vec2
    {
        union
        {
            struct { float x, y; };
            float data[2];
        };

        constexpr Vec2() : x(0), y(0) {}
        constexpr Vec2(float x, float y) : x(x), y(y) {}

        static constexpr std::size_t Length() { return 2; }

        float& operator[](std::size_t i) { return (&x)[i]; }
        const float& operator[](std::size_t i) const { return (&x)[i]; }

        bool operator==(const Vec2& o) const
        {
            return x == o.x && y == o.y;
        }

        bool operator!=(const Vec2& o) const
        {
            return !(*this == o);
        }

        Vec2 operator+(const Vec2& o) const
        {
            return { x + o.x, y + o.y };
        }

        Vec2 operator-(const Vec2& o) const
        {
            return { x - o.x, y - o.y };
        }

        Vec2 operator*(const Vec2& o) const
        {
            return { x * o.x, y * o.y };
        }

        Vec2 operator/(const Vec2& o) const
        {
            return { x / o.x, y / o.y };
        }

        Vec2 operator*(float s) const
        {
            return { x * s, y * s };
        }

        Vec2 operator/(float s) const
        {
            return { x / s, y / s };
        }

        Vec2& operator+=(const Vec2& o)
        {
            x += o.x;
            y += o.y;
            return *this;
        }

        Vec2& operator-=(const Vec2& o)
        {
            x -= o.x;
            y -= o.y;
            return *this;
        }

        Vec2& operator*=(const Vec2& o)
        {
            x *= o.x;
            y *= o.y;
            return *this;
        }

        Vec2& operator/=(const Vec2& o)
        {
            x /= o.x;
            y /= o.y;
            return *this;
        }

        Vec2& operator*=(float s)
        {
            x *= s;
            y *= s;
            return *this;
        }

        Vec2& operator/=(float s)
        {
            x /= s;
            y /= s;
            return *this;
        }
    };
}