#pragma once

#include "Vec2.h"
#include "Vec3.h"
#include "Vec4.h"

namespace Rynox::Math
{
    // ================ VEC2 ================

    // Returns the length (magnitude) of the vector
    inline float Length(const Vec2& v);

    // Returns the squared length of the vector (faster, avoids square root)
    inline float LengthSq(const Vec2& v);

    // Returns a normalized unit vector (direction only, length = 1)
    inline Vec2 Normalize(const Vec2& v);

    // Returns the dot product of two vectors
    inline float Dot(const Vec2& a, const Vec2& b);

    // Returns a perpendicular vector (rotated 90° counter-clockwise)
    inline Vec2 Perpendicular(const Vec2& v);

    // Linear interpolation between two vectors
    inline Vec2 Lerp(const Vec2& a, const Vec2& b, float t);

    // ================ VEC3 ================

    // Returns the length (magnitude) of the vector
    inline float Length(const Vec3& v);

    // Returns the squared length of the vector (faster, avoids square root)
    inline float LengthSq(const Vec3& v);

    // Returns a normalized unit vector (direction only, length = 1)
    inline Vec3 Normalize(const Vec3& v);

    // Returns the dot product of two vectors
    inline float Dot(const Vec3& a, const Vec3& b);

    // Returns the cross product of two vectors (perpendicular vector)
    inline Vec3 Cross(const Vec3& a, const Vec3& b);

    // Returns the distance between two points
    inline float Distance(const Vec3& a, const Vec3& b);

    // Returns the angle between two vectors (in radians)
    inline float Angle(const Vec3& a, const Vec3& b);

    // Projects vector a onto vector b
    inline Vec3 Project(const Vec3& a, const Vec3& b);

    // Reflects a vector off a surface with given normal
    inline Vec3 Reflect(const Vec3& v, const Vec3& normal);

    // Refracts a vector through a surface
    inline Vec3 Refract(const Vec3& v, const Vec3& normal, float eta);

    // Linear interpolation between two vectors
    inline Vec3 Lerp(const Vec3& a, const Vec3& b, float t);

    // ================ VEC4 ================

    // Returns the length (magnitude) of the vector
    inline float Length(const Vec4& v);

    // Returns the squared length of the vector (faster, avoids square root)
    inline float LengthSq(const Vec4& v);

    // Returns a normalized unit vector (direction only, length = 1)
    inline Vec4 Normalize(const Vec4& v);

    // Returns the dot product of two vectors
    inline float Dot(const Vec4& a, const Vec4& b);

    // Linear interpolation between two vectors
    inline Vec4 Lerp(const Vec4& a, const Vec4& b, float t);
}