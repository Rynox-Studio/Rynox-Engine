#pragma once

#include <concepts>

namespace Rynox::Math
{
	template<typename T>
	concept Integral = std::integral<T>;

	template<typename T>
	concept Floating = std::floating_point<T>;

	template<typename T>
	concept Arithmetic = Integral<T> || Floating<T>;
}
