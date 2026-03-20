#pragma once

#include <Math/Vec3.h>

namespace Rynox
{
	struct TransformComponent
	{
		Math::Vec3 position;
		Math::Vec3 rotation;
		Math::Vec3 scale = { 1.0f, 1.0f, 1.0f };
	};
}