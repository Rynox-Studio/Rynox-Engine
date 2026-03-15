#include "Math/MatrixTransforms.h"
#include <cmath>

namespace Rynox::Math
{
    Mat4 LookAt(const Vec3& eye, const Vec3& target, const Vec3& up)
    {
        Vec3 zaxis = Normalize(target - eye);
        Vec3 xaxis = Normalize(Cross(up, zaxis));
        Vec3 yaxis = Cross(zaxis, xaxis);

        Mat4 view = Mat4::Identity();

        view[0][0] = xaxis.x; view[0][1] = xaxis.y; view[0][2] = xaxis.z; view[0][3] = -Dot(xaxis, eye);
        view[1][0] = yaxis.x; view[1][1] = yaxis.y; view[1][2] = yaxis.z; view[1][3] = -Dot(yaxis, eye);
        view[2][0] = zaxis.x; view[2][1] = zaxis.y; view[2][2] = zaxis.z; view[2][3] = -Dot(zaxis, eye);
        view[3][0] = 0.0f;    view[3][1] = 0.0f;    view[3][2] = 0.0f;    view[3][3] = 1.0f;

        return view;
    }

    Mat4 Perspective(float fovY, float aspect, float zNear, float zFar)
    {
        float tanHalfFov = std::tan(fovY * 0.5f);

        Mat4 proj = Mat4::Zero();
        proj[0][0] = 1.0f / (aspect * tanHalfFov);
        proj[1][1] = 1.0f / tanHalfFov;
        proj[2][2] = zFar / (zFar - zNear);
        proj[2][3] = 1.0f;
        proj[3][2] = -(zNear * zFar) / (zFar - zNear);

        return proj;
    }

    Mat4 Ortho(float left, float right, float bottom, float top, float zNear, float zFar)
    {
        Mat4 proj = Mat4::Identity();

        proj[0][0] = 2.0f / (right - left);
        proj[1][1] = 2.0f / (top - bottom);
        proj[2][2] = 1.0f / (zFar - zNear);
        proj[3][0] = -(right + left) / (right - left);
        proj[3][1] = -(top + bottom) / (top - bottom);
        proj[3][2] = -zNear / (zFar - zNear);

        return proj;
    }

    Mat4 Translate(const Vec3& v)
    {
        Mat4 result = Mat4::Identity();
        result[3][0] = v.x;
        result[3][1] = v.y;
        result[3][2] = v.z;
        return result;
    }

    Mat4 Scale(const Vec3& v)
    {
        Mat4 result = Mat4::Identity();
        result[0][0] = v.x;
        result[1][1] = v.y;
        result[2][2] = v.z;
        return result;
    }

    Mat4 RotateX(float angleRad)
    {
        Mat4 result = Mat4::Identity();
        float c = std::cos(angleRad);
        float s = std::sin(angleRad);
        result[1][1] = c; result[1][2] = s;
        result[2][1] = -s; result[2][2] = c;
        return result;
    }

    Mat4 RotateY(float angleRad)
    {
        Mat4 result = Mat4::Identity();
        float c = std::cos(angleRad);
        float s = std::sin(angleRad);
        result[0][0] = c;  result[0][2] = -s;
        result[2][0] = s;  result[2][2] = c;
        return result;
    }

    Mat4 RotateZ(float angleRad)
    {
        Mat4 result = Mat4::Identity();
        float c = std::cos(angleRad);
        float s = std::sin(angleRad);
        result[0][0] = c; result[0][1] = s;
        result[1][0] = -s; result[1][1] = c;
        return result;
    }

    Mat4 Rotate(float angleRad, const Vec3& axis)
    {
        Vec3 normAxis = Normalize(axis);
        float c = std::cos(angleRad);
        float s = std::sin(angleRad);
        float oneMinusC = 1.0f - c;

        float x = normAxis.x;
        float y = normAxis.y;
        float z = normAxis.z;

        Mat4 result = Mat4::Identity();

        result[0][0] = c + x * x * oneMinusC;
        result[0][1] = x * y * oneMinusC + z * s;
        result[0][2] = x * z * oneMinusC - y * s;

        result[1][0] = x * y * oneMinusC - z * s;
        result[1][1] = c + y * y * oneMinusC;
        result[1][2] = y * z * oneMinusC + x * s;

        result[2][0] = x * z * oneMinusC + y * s;
        result[2][1] = y * z * oneMinusC - x * s;
        result[2][2] = c + z * z * oneMinusC;

        return result;
    }

    Mat4 RotateYawPitchRoll(float yawRad, float pitchRad, float rollRad)
    {
        return RotateY(yawRad) * RotateX(pitchRad) * RotateZ(rollRad);
    }

    Mat4 Translate(const Mat4& m, const Vec3& v)
    {
        Mat4 result = m;
        result[3][0] = m[0][0] * v.x + m[1][0] * v.y + m[2][0] * v.z + m[3][0];
        result[3][1] = m[0][1] * v.x + m[1][1] * v.y + m[2][1] * v.z + m[3][1];
        result[3][2] = m[0][2] * v.x + m[1][2] * v.y + m[2][2] * v.z + m[3][2];
        result[3][3] = m[3][3];
        return result;
    }

    Mat4 Scale(const Mat4& m, const Vec3& v)
    {
        Mat4 result = m;
        result[0][0] = m[0][0] * v.x; result[0][1] = m[0][1] * v.x; result[0][2] = m[0][2] * v.x; result[0][3] = m[0][3] * v.x;
        result[1][0] = m[1][0] * v.y; result[1][1] = m[1][1] * v.y; result[1][2] = m[1][2] * v.y; result[1][3] = m[1][3] * v.y;
        result[2][0] = m[2][0] * v.z; result[2][1] = m[2][1] * v.z; result[2][2] = m[2][2] * v.z; result[2][3] = m[2][3] * v.z;
        return result;
    }

    Mat4 RotateX(const Mat4& m, float angleRad)
    {
        return m * RotateX(angleRad);
    }

    Mat4 RotateY(const Mat4& m, float angleRad)
    {
        return m * RotateY(angleRad);
    }

    Mat4 RotateZ(const Mat4& m, float angleRad)
    {
        return m * RotateZ(angleRad);
    }

    Mat4 Rotate(const Mat4& m, float angleRad, const Vec3& axis)
    {
        return m * Rotate(angleRad, axis);
    }

    Mat4 RotateYawPitchRoll(const Mat4& m, float yawRad, float pitchRad, float rollRad)
    {
        return m * RotateYawPitchRoll(yawRad, pitchRad, rollRad);
    }
}