#pragma once

#include <cstdint>
#include "Rendering/VertexLayout.h"

namespace Rynox::Renderer::OpenGL
{
	constexpr uint32_t INVALID_ID = 0xFFFFFFFF;

	struct OpenGLVertexArray
	{
	public:
		uint32_t id = INVALID_ID;
	};

	struct OpenGLVertexBuffer
	{
	public:
		uint32_t id = INVALID_ID;
		uint32_t size = INVALID_ID;
		VertexLayout layout;
	};

	struct OpenGLIndexBuffer
	{
	public:
		uint32_t id = INVALID_ID;
		uint32_t count = INVALID_ID;
	};

	struct OpenGLShader
	{
	public:
		uint32_t id = INVALID_ID;
	};

	struct OpenGLMesh
	{
	public:
		OpenGLVertexArray vao;
		OpenGLVertexBuffer vbo;
		OpenGLIndexBuffer ibo;
		uint32_t count;
	};
}