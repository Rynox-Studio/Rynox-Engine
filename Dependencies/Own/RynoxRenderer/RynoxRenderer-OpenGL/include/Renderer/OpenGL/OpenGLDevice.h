#pragma once

#include <cstdint>
#include "Core/Graphics/RenderTypes.h"
#include "Renderer/OpenGL/OpenGLResources.h"
#include "Core/Graphics/VertexLayout.h"

using namespace Rynox::Graphics;

namespace Rynox::Renderer::OpenGL
{
	class OpenGLDevice
	{
	public:
		static OpenGLVertexBuffer CreateVertexBuffer(const void* data, uint32_t size, Graphics::VertexLayout& layout);
		static OpenGLIndexBuffer CreateIndexBuffer(const uint32_t* data, uint32_t count);
		static OpenGLVertexArray CreateVertexArray(const OpenGLVertexBuffer& vertexBuffer, const OpenGLIndexBuffer& indexBuffer);
		static OpenGLShader CreateShader(const char* vertexSrc, const char* fragmentSrc);

		static void BindVertexBuffer(const OpenGLVertexBuffer& buffer);
		static void BindIndexBuffer(const OpenGLIndexBuffer& buffer);
		static void BindVertexArray(const OpenGLVertexArray& array);
		static void BindShader(const OpenGLShader& shader);

		static void UnBindVertexBuffer();
		static void UnBindIndexBuffer();
		static void UnBindVertexArray();
		static void UnBindShader();
	};
}