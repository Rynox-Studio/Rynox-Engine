#pragma once

#include "Math/Detail/Concepts.h"

#include "Math/Vec/Vec4.h"

namespace Rynox::Math
{
    template<Floating T>
    struct Mat4
    {
        T Data[16];

        Mat4() = default;

        Mat4(
            T m00, T m01, T m02, T m03,
            T m10, T m11, T m12, T m13,
            T m20, T m21, T m22, T m23,
            T m30, T m31, T m32, T m33)
            : Data{
                m00, m01, m02, m03,
                m10, m11, m12, m13,
                m20, m21, m22, m23,
                m30, m31, m32, m33
            } {}

        T& operator()(int row, int col)
        {
            return Data[row * 4 + col];
        }

        const T& operator()(int row, int col) const
        {
            return Data[row * 4 + col];
        }

        Mat4 operator*(const Mat4& o) const
        {
            Mat4 r{};

            for (int i = 0; i < 4; ++i)
            {
                for (int j = 0; j < 4; ++j)
                {
                    for (int k = 0; k < 4; ++k)
                    {
                        r(i, j) += (*this)(i, k) * o(k, j);
                    }
                }
            }

            return r;
        }

        Mat4& operator*=(const Mat4& o)
        {
            *this = *this * o;
            return *this;
        }
    };

    template<Floating T>
    Vec4<T> operator*(const Vec4<T>& v, const Mat4<T>& m)
    {
        return Vec4<T>(
            v.x * m(0, 0) + v.y * m(1, 0) + v.z * m(2, 0) + v.w * m(3, 0),
            v.x * m(0, 1) + v.y * m(1, 1) + v.z * m(2, 1) + v.w * m(3, 1),
            v.x * m(0, 2) + v.y * m(1, 2) + v.z * m(2, 2) + v.w * m(3, 2),
            v.x * m(0, 3) + v.y * m(1, 3) + v.z * m(2, 3) + v.w * m(3, 3)
        );
    }
}
