#pragma once

#include "Vec3.h"
#include "Vec4.h"
#include "Mat4.h"
#include "VectorFunctions.h"

namespace Rynox::Math
{
    // ================ VIEW MATRICES ================
    
    // Left-Handed, Y-up
    inline Mat4 LookAt(const Vec3& eye, const Vec3& target, const Vec3& up);

    // ================ PROJECTION MATRICES ================

    // Left-Handed, Y-up (angle in radians)
    inline Mat4 Perspective(float fovY, float aspect, float zNear, float zFar);

    // Left-Handed, Y-up
    inline Mat4 Ortho(float left, float right, float bottom, float top, float zNear, float zFar);

    // ================ TRANSFORMATION MATRICES (FROM IDENTITY) ================

    inline Mat4 Translate(const Vec3& v);
    inline Mat4 Scale(const Vec3& v);

    inline Mat4 RotateX(float angleRad);
    inline Mat4 RotateY(float angleRad);
    inline Mat4 RotateZ(float angleRad);

    // Rotation around arbitrary axis (angle in radians)
    inline Mat4 Rotate(float angleRad, const Vec3& axis);

    // Rotation from Euler angles (yaw, pitch, roll in radians)
    inline Mat4 RotateYawPitchRoll(float yawRad, float pitchRad, float rollRad);

    // ================ TRANSFORMATION MATRICES (APPLY TO EXISTING) ================

    // Translate an existing matrix
    inline Mat4 Translate(const Mat4& m, const Vec3& v);

    // Scale an existing matrix
    inline Mat4 Scale(const Mat4& m, const Vec3& v);

    inline Mat4 RotateX(const Mat4& m, float angleRad);
    inline Mat4 RotateY(const Mat4& m, float angleRad);
    inline Mat4 RotateZ(const Mat4& m, float angleRad);

    // Rotate an existing matrix around arbitrary axis
    inline Mat4 Rotate(const Mat4& m, float angleRad, const Vec3& axis);

    // Rotate an existing matrix using Euler angles
    inline Mat4 RotateYawPitchRoll(const Mat4& m, float yawRad, float pitchRad, float rollRad);
}