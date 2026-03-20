#include "Math/MatrixTransforms.h"

#include <cmath>

#include <Math/Utility.h>

namespace Rynox::Math
{
    Mat4 LookAtLH(const Vec3& eye, const Vec3& target, const Vec3& up)
    {
        Vec3 zaxis = Normalize(target - eye);
        if (Length(zaxis) < FLOAT_EPS) 
            zaxis = Vec3(0, 0, 1);

        Vec3 xaxis = Normalize(Cross(up, zaxis));
        if (Length(xaxis) < FLOAT_EPS)
            xaxis = Normalize(Cross(Vec3(0, 1, 0), zaxis));

        Vec3 yaxis = Cross(zaxis, xaxis);

        return Mat4(
            xaxis.x, yaxis.x, zaxis.x, 0.0f,
            xaxis.y, yaxis.y, zaxis.y, 0.0f,
            xaxis.z, yaxis.z, zaxis.z, 0.0f,
            -Dot(xaxis, eye),
            -Dot(yaxis, eye),
            -Dot(zaxis, eye),
            1.0f
        );
    }

    Mat4 PerspectiveLH(float fovY, float aspect, float zNear, float zFar)
    {
        float tanHalfFov = std::tan(fovY * 0.5f);

        Mat4 m = Mat4::Zero();

        m(0, 0) = 1.0f / (aspect * tanHalfFov);
        m(1, 1) = 1.0f / tanHalfFov;
        m(2, 2) = zFar / (zFar - zNear);
        m(3, 2) = 1.0f;
        m(2, 3) = -zNear * zFar / (zFar - zNear);

		return m;
    }

    Mat4 OrthoLH(float left, float right, float bottom, float top, float zNear, float zFar)
    {
        Mat4 m = Mat4::Identity();

        m(0, 0) = 2.0f / (right - left);
        m(0, 3) = -((right + left) / (right - left));
        m(1, 1) = 2.0f / (top - bottom);
        m(1, 3) = -((top + bottom) / (top - bottom));
        m(2, 2) = 1.0f / (zFar - zNear);
        m(2, 3) = -(zNear / (zFar - zNear));

        return m;
    }

    Mat4 Translate(const Vec3& v)
    {
        Mat4 m = Mat4::Identity();

        m(0, 3) = v.x;
        m(1, 3) = v.y;
        m(2, 3) = v.z;

        return m;
    }

    Mat4 Scale(const Vec3& v)
    {
        Mat4 m = Mat4::Identity();

        m(0, 0) = v.x;
        m(1, 1) = v.y;
        m(2, 2) = v.z;

        return m;
    }

    Mat4 Rotate(float angleRad, const Vec3& axis)
    {
        Vec3 v = Normalize(axis);
        float x = v.x, y = v.y, z = v.z;
        float c = std::cos(angleRad);
        float s = std::sin(angleRad);
        float t = 1.0f - c;

        Mat4 m = Mat4::Identity();

        m(0, 0) = c + x * x * t;
        m(0, 1) = x * y * t - z * s;
        m(0, 2) = x * z * t + y * s;

        m(1, 0) = y * x * t + z * s;
        m(1, 1) = c + y * y * t;
        m(1, 2) = y * z * t - x * s;

        m(2, 0) = z * x * t - y * s;
        m(2, 1) = z * y * t + x * s;
        m(2, 2) = c + z * z * t;

        return m;
    }

    Mat4 Translate(const Mat4& m, const Vec3& v)
    {
        return Translate(v) * m;
    }

    Mat4 Scale(const Mat4& m, const Vec3& v)
    {
        return Scale(v) * m;
    }

    Mat4 Rotate(const Mat4& m, float angleRad, const Vec3& axis)
    {
        return Rotate(angleRad, axis) * m;
    }
}