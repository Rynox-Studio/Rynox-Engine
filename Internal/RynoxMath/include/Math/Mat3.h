#pragma once

#include "Math/Vec3.h"

namespace Rynox::Math
{
    // column-major
    struct Mat3
    {
        // column-major
        float data[9];

        constexpr Mat3() : data{ 0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f } {}

        constexpr Mat3(
            float m00, float m01, float m02,
            float m10, float m11, float m12,
            float m20, float m21, float m22)
            : data{
                m00, m10, m20,
                m01, m11, m21,
                m02, m12, m22
            }
        {
        }

        static Mat3 Zero()
        {
            return Mat3(
                0, 0, 0,
                0, 0, 0,
                0, 0, 0
            );
        }

        static Mat3 Identity()
        {
            return Mat3(
                1.0f, 0.0f, 0.0f,
                0.0f, 1.0f, 0.0f,
                0.0f, 0.0f, 1.0f
            );
        }

        Mat3 Transpose() const
        {
            return Mat3(
                data[0], data[3], data[6],
                data[1], data[4], data[7],
                data[2], data[5], data[8]
            );
        }

        float& operator()(int row, int col) { return data[col * 3 + row]; }
        const float& operator()(int row, int col) const { return data[col * 3 + row]; }

        Mat3 operator*(const Mat3& o) const
        {
            Mat3 r{};
            for (int i = 0; i < 3; ++i)
                for (int j = 0; j < 3; ++j)
                {
                    r(i, j) = (*this)(i, 0) * o(0, j) +
                        (*this)(i, 1) * o(1, j) +
                        (*this)(i, 2) * o(2, j);
                }
            return r;
        }

        Mat3& operator*=(const Mat3& o)
        {
            *this = *this * o;
            return *this;
        }
    };

    inline Vec3 operator*(const Mat3& m, const Vec3& v)
    {
        return Vec3(
            m(0, 0) * v.x + m(0, 1) * v.y + m(0, 2) * v.z,
            m(1, 0) * v.x + m(1, 1) * v.y + m(1, 2) * v.z,
            m(2, 0) * v.x + m(2, 1) * v.y + m(2, 2) * v.z
        );
    }
}