#pragma once

#include "CameraData.h"
#include "RenderCommand.h"
#include <vector>

namespace Rynox::Graphics
{
	struct FrameContext
	{
		CameraData camera;
		std::vector<RenderCommand> commands;
		float time;
	};
}