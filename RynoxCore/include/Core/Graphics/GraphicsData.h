#pragma once

#include "Core/Graphics/VertexLayout.h"

namespace Rynox::Graphics
{
	struct MeshData
	{
	public:
		const void* vertices;
		uint32_t vertexSize;
		const uint32_t* indices;
		uint32_t indexCount;
		VertexLayout layout;
	};

	struct ShaderData
	{
	public:
		const char* vertCode;
		const char* fragCode;
	};
}