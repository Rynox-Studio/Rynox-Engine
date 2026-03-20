#pragma once

#include <cstddef>

namespace Rynox::Math
{
    struct Vec3
    {
        union
        {
            struct { float x, y, z; };
            float data[3];
        };

        constexpr Vec3() : x(0), y(0), z(0) {}
        constexpr Vec3(float x, float y, float z) : x(x), y(y), z(z) {}

        static constexpr std::size_t Length() { return 3; }

        float& operator[](std::size_t i) { return (&x)[i]; }
        const float& operator[](std::size_t i) const { return (&x)[i]; }

        bool operator==(const Vec3& o) const
        {
            return x == o.x && y == o.y && z == o.z;
        }

        bool operator!=(const Vec3& o) const
        {
            return !(*this == o);
        }

        Vec3 operator+(const Vec3& o) const
        {
            return { x + o.x, y + o.y, z + o.z };
        }

        Vec3 operator-(const Vec3& o) const
        {
            return { x - o.x, y - o.y, z - o.z };
        }

        Vec3 operator*(const Vec3& o) const
        {
            return { x * o.x, y * o.y, z * o.z };
        }

        Vec3 operator/(const Vec3& o) const
        {
            return { x / o.x, y / o.y, z / o.z };
        }

        Vec3 operator*(float s) const
        {
            return { x * s, y * s, z * s };
        }

        Vec3 operator/(float s) const
        {
            return { x / s, y / s, z / s };
        }

        Vec3& operator+=(const Vec3& o)
        {
            x += o.x;
            y += o.y;
            z += o.z;
            return *this;
        }

        Vec3& operator-=(const Vec3& o)
        {
            x -= o.x;
            y -= o.y;
            z -= o.z;
            return *this;
        }

        Vec3& operator*=(const Vec3& o)
        {
            x *= o.x;
            y *= o.y;
            z *= o.z;
            return *this;
        }

        Vec3& operator/=(const Vec3& o)
        {
            x /= o.x;
            y /= o.y;
            z /= o.z;
            return *this;
        }

        Vec3& operator*=(float s)
        {
            x *= s;
            y *= s;
            z *= s;
            return *this;
        }

        Vec3& operator/=(float s)
        {
            x /= s;
            y /= s;
            z /= s;
            return *this;
        }
    };
}