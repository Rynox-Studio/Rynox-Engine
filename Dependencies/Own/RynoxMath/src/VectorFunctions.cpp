#include "Math/VectorFunctions.h"

#include <cmath>

namespace Rynox::Math
{
    float Length(const Vec2& v)
    {
        return std::sqrt(v.x * v.x + v.y * v.y);
    }

    float LengthSq(const Vec2& v)
    {
        return v.x * v.x + v.y * v.y;
    }

    Vec2 Normalize(const Vec2& v)
    {
        float len = Length(v);
        if (len == 0.0f) return Vec2{ 0,0 };
        return Vec2{ v.x / len, v.y / len };
    }

    float Dot(const Vec2& a, const Vec2& b)
    {
        return a.x * b.x + a.y * b.y;
    }

    Vec2 Perpendicular(const Vec2& v)
    {
        return Vec2{ -v.y, v.x };
    }

    Vec2 Lerp(const Vec2& a, const Vec2& b, float t)
    {
        return Vec2{ a.x + (b.x - a.x) * t, a.y + (b.y - a.y) * t };
    }

    float Length(const Vec3& v)
    {
        return std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
    }

    float LengthSq(const Vec3& v)
    {
        return v.x * v.x + v.y * v.y + v.z * v.z;
    }

    Vec3 Normalize(const Vec3& v)
    {
        float len = Length(v);
        if (len == 0.0f) return Vec3{ 0,0,0 };
        return Vec3{ v.x / len, v.y / len, v.z / len };
    }

    float Dot(const Vec3& a, const Vec3& b)
    {
        return a.x * b.x + a.y * b.y + a.z * b.z;
    }

    Vec3 Cross(const Vec3& a, const Vec3& b)
    {
        return Vec3{
            a.y * b.z - a.z * b.y,
            a.z * b.x - a.x * b.z,
            a.x * b.y - a.y * b.x
        };
    }

    float Distance(const Vec3& a, const Vec3& b)
    {
        return Length(a - b);
    }

    float Angle(const Vec3& a, const Vec3& b)
    {
        float dot = Dot(a, b);
        float lenA = Length(a);
        float lenB = Length(b);
        if (lenA == 0 || lenB == 0) return 0;
        return std::acos(dot / (lenA * lenB));
    }

    Vec3 Project(const Vec3& a, const Vec3& b)
    {
        float lenSq = LengthSq(b);
        if (lenSq == 0) return Vec3{ 0,0,0 };
        return b * (Dot(a, b) / lenSq);
    }

    Vec3 Reflect(const Vec3& v, const Vec3& normal)
    {
        return v - normal * (2.0f * Dot(v, normal));
    }

    Vec3 Refract(const Vec3& v, const Vec3& normal, float eta)
    {
        float dot = Dot(v, normal);
        float k = 1.0f - eta * eta * (1.0f - dot * dot);
        if (k < 0.0f) return Vec3{ 0,0,0 };
        return v * eta - normal * (eta * dot + std::sqrt(k));
    }

    Vec3 Lerp(const Vec3& a, const Vec3& b, float t)
    {
        return Vec3{ a.x + (b.x - a.x) * t, a.y + (b.y - a.y) * t, a.z + (b.z - a.z) * t };
    }

    float Length(const Vec4& v)
    {
        return std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z + v.w * v.w);
    }

    float LengthSq(const Vec4& v)
    {
        return v.x * v.x + v.y * v.y + v.z * v.z + v.w * v.w;
    }

    Vec4 Normalize(const Vec4& v)
    {
        float len = Length(v);
        if (len == 0.0f) return Vec4{ 0,0,0,0 };
        return Vec4{ v.x / len, v.y / len, v.z / len, v.w / len };
    }

    float Dot(const Vec4& a, const Vec4& b)
    {
        return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
    }

    Vec4 Lerp(const Vec4& a, const Vec4& b, float t)
    {
        return Vec4{
            a.x + (b.x - a.x) * t,
            a.y + (b.y - a.y) * t,
            a.z + (b.z - a.z) * t,
            a.w + (b.w - a.w) * t
        };
    }
}