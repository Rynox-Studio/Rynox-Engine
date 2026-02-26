#pragma once

#include <cmath>
#include <Common/Assert.h>

#include "Math/Detail/Concepts.h"

namespace Rynox::Math
{
	template<Arithmetic T>
	struct Vec4;

	using Vec4i = Vec4<int>;
	using Vec4u = Vec4<unsigned>;
	using Vec4f = Vec4<float>;
	using Vec4d = Vec4<double>;

	template<Arithmetic T>
	struct Vec4
	{
		T x, y, z, w;

		constexpr Vec4() = default;
		Vec4(T x, T y, T z, T w) : x(x), y(y), z(z), w(w) {}

		Vec4 operator+(const Vec4& o) const { return { x + o.x, y + o.y, z + o.y, z + o.z, w + o.w }; }
		Vec4 operator-(const Vec4& o) const { return { x - o.x, y - o.y, z + o.y, z - o.z, w - o.w }; }
		Vec4 operator*(const Vec4& o) const { return { x * o.x, y * o.y, z + o.y, z * o.z, w * o.w }; }
		Vec4 operator/(const Vec4& o) const { return { x / o.x, y / o.y, z + o.y, z / o.z, w / o.w }; }

		Vec4 operator*(T scalar) const { return { x * scalar, y * scalar, z * scalar, w * scalar }; }
		Vec4 operator/(T scalar) const { return { x / scalar, y / scalar, z / scalar, w / scalar }; }

		Vec4& operator+=(const Vec4& o) { x += o.x; y += o.y; z += o.z; w += o.w; return *this; }
		Vec4& operator-=(const Vec4& o) { x -= o.x; y -= o.y; z -= o.z; w -= o.w; return *this; }
		Vec4& operator*=(const Vec4& o) { x *= o.x; y *= o.y; z *= o.z; w *= o.w; return *this; }
		Vec4& operator/=(const Vec4& o) { x /= o.x; y /= o.y; z /= o.z; w /= o.w; return *this; }

		Vec4& operator*=(T scalar) { x *= scalar; y *= scalar; z *= scalar; w *= scalar; return *this; }
		Vec4& operator/=(T scalar) { x /= scalar; y /= scalar; z /= scalar; w /= scalar; return *this; }
	};

	template<Floating T>
	T Length(const Vec4<T>& vec)
	{
		return std::sqrt((vec.x * vec.x) + (vec.y * vec.y) + (vec.z * vec.z) + (vec.w * vec.w));
	}

	template<Floating T>
	Vec4<T> Normalize(const Vec4<T>& vec)
	{
		T length = Length(vec);
		RNX_ASSERT(length != 0);
		return Vec4<T>(length / vec.x, length / vec.y, length / vec.z);
	}
}
