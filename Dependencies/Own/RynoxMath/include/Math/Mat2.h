#pragma once

#include "Math/Vec2.h"

namespace Rynox::Math
{
    // column-major
    struct Mat2
    {
        // column-major
        float data[4];

        constexpr Mat2() : data{ 0.0f, 0.0f, 0.0f, 0.0f } {}

        constexpr Mat2(
            float m00, float m01,
            float m10, float m11)
            : data{ m00, m10, m01, m11 }
        {
        }

        static Mat2 Zero()
        {
            return Mat2(
                0, 0,
                0, 0
            );
        }

        static Mat2 Identity()
        {
            return Mat2(
                1.0f, 0.0f,
                0.0f, 1.0f
            );
        }

        Mat2 Transpose() const
        {
            return Mat2(
                data[0], data[2],
                data[1], data[3]
            );
        }

        float* operator[](int col) { return &data[col * 2]; }
        const float* operator[](int col) const { return &data[col * 2]; }

        float& operator()(int row, int col) { return data[col * 2 + row]; }
        const float& operator()(int row, int col) const { return data[col * 2 + row]; }

        Mat2 operator*(const Mat2& o) const
        {
            Mat2 r{};
            for (int i = 0; i < 2; ++i)
                for (int j = 0; j < 2; ++j)
                {
                    r(i, j) = (*this)(i, 0) * o(0, j) +
                        (*this)(i, 1) * o(1, j);
                }
            return r;
        }

        Mat2& operator*=(const Mat2& o)
        {
            *this = *this * o;
            return *this;
        }
    };

    inline Vec2 operator*(const Mat2& m, const Vec2& v)
    {
        return Vec2(
            m(0, 0) * v.x + m(0, 1) * v.y,
            m(1, 0) * v.x + m(1, 1) * v.y
        );
    }
}