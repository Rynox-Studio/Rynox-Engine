#pragma once

#include <vector>
#include <Math/Mat4.h>
#include "RenderItem.h"
#include "Core/Graphics/RenderTypes.h"

namespace Rynox::Graphics
{
	struct RenderCommand
	{
		std::vector<RenderItem> items;
		Math::Mat4 worldMatrix;
		Graphics::ShaderHandle shader;
	};
}