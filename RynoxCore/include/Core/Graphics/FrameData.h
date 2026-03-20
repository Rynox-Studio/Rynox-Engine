#pragma once

#include <Math/Mat4.h>

namespace Rynox::Graphics
{
	struct CameraData
	{
		Math::Mat4 projMatrixLH;
		Math::Mat4 viewMatrixLH;
	};

	struct FrameData
	{
		CameraData camera;
		float time;
	};
}