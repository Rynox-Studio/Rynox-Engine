#pragma once

#include <vector>
#include <Math/Mat/Mat4.h>
#include "RenderItem.h"

namespace Rynox::Graphics
{
	struct RenderCommand
	{
		std::vector<RenderItem> items;
		Math::Mat4<float> worldMatrix;
	};
}