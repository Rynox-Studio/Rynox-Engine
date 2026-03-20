#pragma once

#include "Math/Vec4.h"

namespace Rynox::Math
{
    // column-major
    struct Mat4
    {
        // column-major
        float data[16];

        constexpr Mat4()
            : data{ 0.0f,0.0f,0.0f,0.0f,
                   0.0f,0.0f,0.0f,0.0f,
                   0.0f,0.0f,0.0f,0.0f,
                   0.0f,0.0f,0.0f,0.0f }
        {
        }

        constexpr Mat4(
            float m00, float m10, float m20, float m30,
            float m01, float m11, float m21, float m31,
            float m02, float m12, float m22, float m32,
            float m03, float m13, float m23, float m33
        )
            : data{
                m00, m10, m20, m30,
                m01, m11, m21, m31,
                m02, m12, m22, m32,
                m03, m13, m23, m33
            }
        {
        }

        static Mat4 Zero()
        {
            return Mat4(
                0, 0, 0, 0,
                0, 0, 0, 0,
                0, 0, 0, 0,
                0, 0, 0, 0
            );
        }

        static Mat4 Identity()
        {
            return Mat4(
                1, 0, 0, 0,
                0, 1, 0, 0,
                0, 0, 1, 0,
                0, 0, 0, 1
            );
        }

        Mat4 Transpose() const
        {
            return Mat4(
                data[0], data[4], data[8], data[12],
                data[1], data[5], data[9], data[13],
                data[2], data[6], data[10], data[14],
                data[3], data[7], data[11], data[15]
            );
        }

        float& operator()(int row, int col) { return data[col * 4 + row]; }
        const float& operator()(int row, int col) const { return data[col * 4 + row]; }

        Mat4 operator*(const Mat4& o) const
        {
            Mat4 r{};
            for (int row = 0; row < 4; ++row)
                for (int col = 0; col < 4; ++col)
                    r(row, col) = (*this)(row, 0) * o(0, col) +
                    (*this)(row, 1) * o(1, col) +
                    (*this)(row, 2) * o(2, col) +
                    (*this)(row, 3) * o(3, col);
            return r;
        }

        Mat4& operator*=(const Mat4& o)
        {
            *this = *this * o;
            return *this;
        }
    };

    inline Vec4 operator*(const Mat4& m, const Vec4& v)
    {
        return Vec4(
            m(0, 0) * v.x + m(0, 1) * v.y + m(0, 2) * v.z + m(0, 3) * v.w,
            m(1, 0) * v.x + m(1, 1) * v.y + m(1, 2) * v.z + m(1, 3) * v.w,
            m(2, 0) * v.x + m(2, 1) * v.y + m(2, 2) * v.z + m(2, 3) * v.w,
            m(3, 0) * v.x + m(3, 1) * v.y + m(3, 2) * v.z + m(3, 3) * v.w
        );
    }
}