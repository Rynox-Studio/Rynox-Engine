#pragma once

#include <cstdint>

#include <Renderer/Dx12VertexBuffer.h>
#include <Renderer/Dx12IndexBuffer.h>

namespace Rynox::Renderer
{
	struct Dx12Mesh
	{
		Dx12VertexBuffer VertexBuffer;
		Dx12IndexBuffer IndexBuffer;
		uint32_t gen;
	};
}
