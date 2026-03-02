#pragma once

#include "Core/Graphics/VertexLayout.h"

namespace Rynox::Graphics
{
	struct MeshData
	{
		const void* vertices;
		uint32_t vertexSize;
		const uint32_t* indices;
		uint32_t indexCount;
		VertexLayout& layout;
	};
}