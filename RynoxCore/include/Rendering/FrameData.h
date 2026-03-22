#pragma once

#include <Math/Mat4.h>

namespace Rynox
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