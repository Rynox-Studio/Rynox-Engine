#pragma once

#include <Math/Mat/Mat4.h>
#include <Math/Vec/Vec3.h>

namespace Rynox::Graphics
{
	struct CameraData
	{
		Math::Mat4<float> projMatrix;
		Math::Mat4<float> viewMatrix;
		Math::Vec3<float> pos;
		Math::Vec3<float> dir;
	};
}