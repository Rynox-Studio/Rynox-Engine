#pragma once

#include <Core/Graphics/RenderTypes.h>

namespace Rynox::Graphics
{
	struct RenderItem
	{
		Graphics::MeshHandle mesh;
		Graphics::ShaderHandle shader;
	};
}