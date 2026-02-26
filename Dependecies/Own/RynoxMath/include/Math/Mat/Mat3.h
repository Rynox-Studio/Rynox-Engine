#pragma once

#include "Math/Detail/Concepts.h"

#include "Math/Vec/Vec3.h"

namespace Rynox::Math
{
    template<Floating T>
    struct Mat3
    {
        T Data[9];

        Mat3() = default;

        Mat3(
            T m00, T m01, T m02,
            T m10, T m11, T m12,
            T m20, T m21, T m22)
            : Data{
                m00, m01, m02,
                m10, m11, m12,
                m20, m21, m22
            } {
        }

        T& operator()(int row, int col)
        {
            return Data[row * 3 + col];
        }

        const T& operator()(int row, int col) const
        {
            return Data[row * 3 + col];
        }

        Mat3 operator*(const Mat3& o) const
        {
            Mat3 r{};

            for (int i = 0; i < 3; ++i)
            {
                for (int j = 0; j < 3; ++j)
                {
                    for (int k = 0; k < 3; ++k)
                    {
                        r(i, j) += (*this)(i, k) * o(k, j);
                    }
                }
            }

            return r;
        }

        Mat3& operator*=(const Mat3& o)
        {
            *this = *this * o;
            return *this;
        }
    };

    template<Floating T>
    Vec3<T> operator*(const Vec3<T>& v, const Mat3<T>& m)
    {
        return Vec3<T>(
            v.x * m(0, 0) + v.y * m(1, 0) + v.z * m(2, 0),
            v.x * m(0, 1) + v.y * m(1, 1) + v.z * m(2, 1),
            v.x * m(0, 2) + v.y * m(1, 2) + v.z * m(2, 2)
        );
    }
}
