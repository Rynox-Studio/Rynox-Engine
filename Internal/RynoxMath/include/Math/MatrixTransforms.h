#pragma once

#include "Vec3.h"
#include "Vec4.h"
#include "Mat4.h"
#include "VectorFunctions.h"

namespace Rynox::Math
{    
    Mat4 LookAtLH(const Vec3& eye, const Vec3& target, const Vec3& up);

    Mat4 PerspectiveLH(float fovYRad, float aspect, float zNear, float zFar);

    Mat4 OrthoLH(float left, float right, float bottom, float top, float zNear, float zFar);

    Mat4 Translate(const Vec3& v);

    Mat4 Scale(const Vec3& v);

    Mat4 Rotate(float angleRad, const Vec3& axis);

    Mat4 Translate(const Mat4& m, const Vec3& v);

    Mat4 Scale(const Mat4& m, const Vec3& v);

    Mat4 Rotate(const Mat4& m, float angleRad, const Vec3& axis);
}