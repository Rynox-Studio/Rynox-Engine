#pragma once

#include <Math/Mat4.h>
#include <Math/Vec3.h>

namespace Rynox::Graphics
{
	struct CameraData
	{
		Math::Mat4 projMatrix;
		Math::Mat4 viewMatrix;
		Math::Vec3 pos;
		Math::Vec3 dir;
	};
}