#pragma once

#include <cmath>
#include <Common/Assert.h>

#include "Math/Detail/Concepts.h"

namespace Rynox::Math
{
	template<Arithmetic T>
	struct Vec2;

	using Vec2i = Vec2<int>;
	using Vec2u = Vec2<unsigned>;
	using Vec2f = Vec2<float>;
	using Vec2d = Vec2<double>;

	template<Arithmetic T>
	struct Vec2
	{
		T x, y;

		constexpr Vec2() = default;
		Vec2(T x, T y) : x(x), y(y) {}

		bool operator==(const Vec2& o) const { return (x == o.x && y == o.y); }
		bool operator!=(const Vec2& o) const { return (x == o.x && y == o.y); }

		Vec2 operator+(const Vec2& o) const { return { x + o.x, y + o.y }; }
		Vec2 operator-(const Vec2& o) const { return { x - o.x, y - o.y }; }
		Vec2 operator*(const Vec2& o) const { return { x * o.x, y * o.y }; }
		Vec2 operator/(const Vec2& o) const { return { x / o.x, y / o.y }; }

		Vec2 operator*(T scalar) const { return { x * scalar, y * scalar }; }
		Vec2 operator/(T scalar) const { return { x / scalar, y / scalar }; }

		Vec2& operator+=(const Vec2& o) { x += o.x; y += o.y; return *this; }
		Vec2& operator-=(const Vec2& o) { x -= o.x; y -= o.y; return *this; }
		Vec2& operator*=(const Vec2& o) { x *= o.x; y *= o.y; return *this; }
		Vec2& operator/=(const Vec2& o) { x /= o.x; y /= o.y; return *this; }

		Vec2& operator*=(T scalar) { x *= scalar; y *= scalar; return *this; }
		Vec2& operator/=(T scalar) { x /= scalar; y /= scalar; return *this; }
	};

	template<Floating T>
	T Length(const Vec2<T>& vec)
	{
		return std::sqrt((vec.x * vec.x) + (vec.y * vec.y));
	}

	template<Floating T>
	Vec2<T> Normalize(const Vec2<T>& vec)
	{
		T length = Length(vec);
		RNX_ASSERT(length != 0);
		return Vec2<T>(length / vec.x, length / vec.y);
	}
}
