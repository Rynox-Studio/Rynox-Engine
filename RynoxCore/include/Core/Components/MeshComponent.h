#pragma once

#include <Core/Graphics/GPUHandle.h>

namespace Rynox
{
	struct MeshComponent
	{
		uint32_t assetID;
		Graphics::MeshHandle meshHandle;
	};
}