#pragma once

#include <cstdint>
#include <vector>
#include "Renderer/RenderTypes.h"
#include "Renderer/OpenGL/OpenGLResource.h"
#include "Renderer/Graphics/VertexLayout.h"

namespace Rynox::Renderer::OpenGL
{
	class OpenGLDevice
	{
	public:
		VertexBufferHandle CreateVertexBuffer(const void* data, uint32_t size, Graphics::VertexLayout& layout);
		IndexBufferHandle CreateIndexBuffer(const uint32_t* data, uint32_t count);
		VertexArrayHandle CreateVertexArray(VertexBufferHandle vertexHandle, IndexBufferHandle indexHandle);
		ShaderHandle CreateShader(const char* vertexSrc, const char* fragmentSrc);

		void BindVertexBuffer(VertexBufferHandle handle);
		void BindIndexBuffer(IndexBufferHandle handle);
		void BindVertexArray(VertexArrayHandle handle);
		void BindShader(ShaderHandle handle);

		void UnBindVertexBuffer();
		void UnBindIndexBuffer();
		void UnBindVertexArray();
		void UnBindShader();
	private:
		std::vector<OpenGLVertexBuffer> m_VertexBuffers;
		std::vector<OpenGLIndexBuffer>  m_IndexBuffers;
		std::vector<OpenGLVertexArray>  m_VertexArrays;
		std::vector<OpenGLShader> m_shaders;
	};
}