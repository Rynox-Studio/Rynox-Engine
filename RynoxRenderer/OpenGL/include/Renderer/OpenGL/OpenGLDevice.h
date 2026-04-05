#pragma once

#include <cstdint>
#include "Renderer/OpenGL/OpenGLResources.h"
#include "Rendering/VertexLayout.h"

namespace Rynox::Renderer::OpenGL
{
	class OpenGLDevice
	{
	public:
		static OpenGLVertexBuffer CreateVertexBuffer(const void* data, uint32_t size, VertexLayout& layout);
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

		static void DrawElements(uint32_t count, uint32_t offset);
		static void DrawElementsBaseVertex(uint32_t count, uint32_t offset, int32_t baseVertex);
		static void UniformMatrix4fv(const OpenGLShader& shader, const char* name, const float* data);
	};
}