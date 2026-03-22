#pragma once

#include <Rendering/GPUHandle.h>

namespace Rynox
{
	struct MeshComponent
	{
		uint32_t assetID;
		Graphics::MeshHandle meshHandle;
	};
}