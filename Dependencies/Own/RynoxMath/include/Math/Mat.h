#pragma once

#include "Mat/Mat2.h"
#include "Mat/Mat3.h"
#include "Mat/Mat4.h"

namespace Rynox::Math
{
    template<typename T>
    inline Mat3<T> ToMat3(const Mat2<T>& m)
    {
        return Mat3<T>(
            m(0,0), m(0,1), 0,
            m(1,0), m(1,1), 0,
            0,      0,      1
        );
    }

    template<typename T>
    inline Mat4<T> ToMat4(const Mat2<T>& m)
    {
        return Mat4<T>(
            m(0,0), m(0,1), 0, 0,
            m(1,0), m(1,1), 0, 0,
            0,      0,      1, 0,
            0,      0,      0, 1
        );
    }

    template<typename T>
    inline Mat2<T> ToMat2(const Mat3<T>& m)
    {
        return Mat2<T>(
            m(0,0), m(0,1),
            m(1,0), m(1,1)
        );
    }

    template<typename T>
    inline Mat4<T> ToMat4(const Mat3<T>& m)
    {
        return Mat4<T>(
            m(0,0), m(0,1), m(0,2), 0,
            m(1,0), m(1,1), m(1,2), 0,
            m(2,0), m(2,1), m(2,2), 0,
            0,      0,      0,      1
        );
    }

    template<typename T>
    inline Mat2<T> ToMat2(const Mat4<T>& m)
    {
        return Mat2<T>(
            m(0,0), m(0,1),
            m(1,0), m(1,1)
        );
    }

    template<typename T>
    inline Mat3<T> ToMat3(const Mat4<T>& m)
    {
        return Mat3<T>(
            m(0,0), m(0,1), m(0,2),
            m(1,0), m(1,1), m(1,2),
            m(2,0), m(2,1), m(2,2)
        );
    }
}