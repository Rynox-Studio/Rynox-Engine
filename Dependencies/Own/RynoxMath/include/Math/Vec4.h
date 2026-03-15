#pragma once

#include <cstddef>

namespace Rynox::Math
{
    struct Vec4
    {
        union
        {
            struct { float x, y, z, w; };
            struct { float r, g, b, a; };
            float data[4];
        };

        constexpr Vec4() : x(0), y(0), z(0), w(0) {}
        constexpr Vec4(float x, float y, float z, float w)
            : x(x), y(y), z(z), w(w) {
        }

        static constexpr std::size_t Length() { return 4; }

        float& operator[](std::size_t i) { return (&x)[i]; }
        const float& operator[](std::size_t i) const { return (&x)[i]; }

        bool operator==(const Vec4& o) const
        {
            return x == o.x && y == o.y && z == o.z && w == o.w;
        }

        bool operator!=(const Vec4& o) const
        {
            return !(*this == o);
        }

        Vec4 operator+(const Vec4& o) const
        {
            return { x + o.x, y + o.y, z + o.z, w + o.w };
        }

        Vec4 operator-(const Vec4& o) const
        {
            return { x - o.x, y - o.y, z - o.z, w - o.w };
        }

        Vec4 operator*(const Vec4& o) const
        {
            return { x * o.x, y * o.y, z * o.z, w * o.w };
        }

        Vec4 operator/(const Vec4& o) const
        {
            return { x / o.x, y / o.y, z / o.z, w / o.w };
        }

        Vec4 operator*(float s) const
        {
            return { x * s, y * s, z * s, w * s };
        }

        Vec4 operator/(float s) const
        {
            return { x / s, y / s, z / s, w / s };
        }

        Vec4& operator+=(const Vec4& o)
        {
            x += o.x;
            y += o.y;
            z += o.z;
            w += o.w;
            return *this;
        }

        Vec4& operator-=(const Vec4& o)
        {
            x -= o.x;
            y -= o.y;
            z -= o.z;
            w -= o.w;
            return *this;
        }

        Vec4& operator*=(const Vec4& o)
        {
            x *= o.x;
            y *= o.y;
            z *= o.z;
            w *= o.w;
            return *this;
        }

        Vec4& operator/=(const Vec4& o)
        {
            x /= o.x;
            y /= o.y;
            z /= o.z;
            w /= o.w;
            return *this;
        }

        Vec4& operator*=(float s)
        {
            x *= s;
            y *= s;
            z *= s;
            w *= s;
            return *this;
        }

        Vec4& operator/=(float s)
        {
            x /= s;
            y /= s;
            z /= s;
            w /= s;
            return *this;
        }
    };
}