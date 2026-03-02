#pragma once

#include "Renderer/RenderTypes.h"

namespace Rynox::Renderer::OpenGL
{
	struct OpenGLMesh
	{
	public:
		VertexBufferHandle vbo;
		IndexBufferHandle ibo;
		VertexArrayHandle vao;
		uint32_t indexCount;

		OpenGLMesh() = default;
		OpenGLMesh(VertexBufferHandle vertexHandle, IndexBufferHandle indexHandel, VertexArrayHandle arrayHandle, uint32_t indexCount)
			: vbo(vertexHandle),
			ibo(indexHandel),
			vao(arrayHandle),
			indexCount(indexCount)
		{
		}
	};
}