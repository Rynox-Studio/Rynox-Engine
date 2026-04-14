#pragma once

#include "Vec2.h"
#include "Vec3.h"
#include "Vec4.h"

namespace Rynox::Math
{
    // ================ VEC2 ================

    float Length(const Vec2& v);

    float LengthSq(const Vec2& v);

    Vec2 Normalize(const Vec2& v);

    float Dot(const Vec2& a, const Vec2& b);

    Vec2 Perpendicular(const Vec2& v);

    Vec2 Lerp(const Vec2& a, const Vec2& b, float t);

    // ================ VEC3 ================

    float Length(const Vec3& v);

    float LengthSq(const Vec3& v);

    Vec3 Normalize(const Vec3& v);

    float Dot(const Vec3& a, const Vec3& b);

    Vec3 Cross(const Vec3& a, const Vec3& b);

    float Distance(const Vec3& a, const Vec3& b);

    float Angle(const Vec3& a, const Vec3& b);

    Vec3 Project(const Vec3& a, const Vec3& b);

    Vec3 Reflect(const Vec3& v, const Vec3& normal);

    Vec3 Refract(const Vec3& v, const Vec3& normal, float eta);

    Vec3 Lerp(const Vec3& a, const Vec3& b, float t);

    // ================ VEC4 ================

    float Length(const Vec4& v);

    float LengthSq(const Vec4& v);

    Vec4 Normalize(const Vec4& v);

    float Dot(const Vec4& a, const Vec4& b);

    Vec4 Lerp(const Vec4& a, const Vec4& b, float t);
}