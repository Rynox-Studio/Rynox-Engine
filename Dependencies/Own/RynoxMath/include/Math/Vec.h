#pragma once

#include "Vec/Vec2.h"
#include "Vec/Vec3.h"
#include "Vec/Vec4.h"

namespace Rynox::Math
{
	template<typename T>
	inline Vec2<T> ToVec2(Vec3<T> o)
	{
		return { o.x, o.y };
	}

	template<typename T>
	inline Vec2<T> ToVec2(Vec4<T> o)
	{
		return { o.x, o.y };
	}

	template<typename T>
	inline Vec3<T> ToVec3(Vec2<T> o, T z = 0)
	{
		return { o.x, o.y, z };
	}

	template<typename T>
	inline Vec3<T> ToVec3(Vec4<T> o)
	{
		return { o.x, o.y, o.z };
	}

	template<typename T>
	inline Vec4<T> ToVec4(Vec2<T> o, T z = 0, T w = 0)
	{
		return { o.x, o.y, z, w };
	}

	template<typename T>
	inline Vec4<T> ToVec4(Vec3<T> o, T w = 0)
	{
		return { o.x, o.y, o.z, w };
	}
}
