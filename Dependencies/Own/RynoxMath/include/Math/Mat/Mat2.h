#pragma once

#include "Math/Detail/Concepts.h"

#include "Math/Vec/Vec2.h"

namespace Rynox::Math
{
	template<Floating T>
	struct Mat2
	{
		T Data[4];

		Mat2() = default;
		Mat2(T m00, T m01, T m10, T m11)
			: Data{ m00, m01, m10, m11 } {
		}

		T* operator[](int row)
		{
			return &Data[row * 2];
		}

		const T* operator[](int row) const
		{
			return &Data[row * 2];
		}

		T& operator()(int row, int col)
		{
			return Data[row * 2 + col];
		}

		const T& operator()(int row, int col) const
		{
			return Data[row * 2 + col];
		}

		Mat2 operator*(const Mat2& o) const
		{
			Mat2 r{};

			r(0, 0) = (*this)(0, 0) * o(0, 0) + (*this)(0, 1) * o(1, 0);
			r(0, 1) = (*this)(0, 0) * o(0, 1) + (*this)(0, 1) * o(1, 1);

			r(1, 0) = (*this)(1, 0) * o(0, 0) + (*this)(1, 1) * o(1, 0);
			r(1, 1) = (*this)(1, 0) * o(0, 1) + (*this)(1, 1) * o(1, 1);

			return r;
		}

		Mat2& operator*=(const Mat2& o)
		{
			*this = *this * o;
			return *this;
		}
	};

	template<Floating T>
	Vec2<T> operator*(const Vec2<T>& v, const Mat2<T>& m)
	{
		return Vec2<T>(
			v.x * m(0, 0) + v.y * m(1, 0),
			v.x * m(0, 1) + v.y * m(1, 1)
		);
	}
}
