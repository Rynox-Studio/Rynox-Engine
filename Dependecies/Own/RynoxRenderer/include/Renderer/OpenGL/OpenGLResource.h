#pragma once

#include <cstdint>
#include "Renderer/Graphics/VertexLayout.h"

namespace Rynox::Renderer::OpenGL
{
	struct OpenGLVertexArray
	{
	public:
		uint32_t id;
	};

	struct OpenGLVertexBuffer
	{
	public:
		uint32_t id;
		uint32_t size;
		Graphics::VertexLayout layout;
	};

	struct OpenGLIndexBuffer
	{
	public:
		uint32_t id;
		uint32_t count;
	};

	struct OpenGLShader
	{
	public:
		uint32_t id;
	};
}