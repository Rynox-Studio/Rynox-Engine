#include "Renderer/OpenGL/OpenGLDevice.h"

#include <glad/glad.h>
#include <Common/Logger.h>

namespace Rynox::Renderer::OpenGL
{
	constexpr GLenum ShaderDataTypeToOpenGLBaseType(Graphics::ShaderDataType type)
	{
		switch (type)
		{
		case Graphics::ShaderDataType::Float:   return GL_FLOAT;
		case Graphics::ShaderDataType::Float2:  return GL_FLOAT;
		case Graphics::ShaderDataType::Float3:  return GL_FLOAT;
		case Graphics::ShaderDataType::Float4:  return GL_FLOAT;
		case Graphics::ShaderDataType::Int:     return GL_INT;
		case Graphics::ShaderDataType::Int2:    return GL_INT;
		case Graphics::ShaderDataType::Int3:    return GL_INT;
		case Graphics::ShaderDataType::Int4:    return GL_INT;
		case Graphics::ShaderDataType::Mat3:    return GL_FLOAT;
		case Graphics::ShaderDataType::Mat4:    return GL_FLOAT;
		case Graphics::ShaderDataType::Bool:    return GL_BOOL;
		}
		return 0;
	}
	VertexBufferHandle OpenGLDevice::CreateVertexBuffer(const void* data, uint32_t size, Graphics::VertexLayout& layout)
	{
		OpenGL::OpenGLVertexBuffer buffer;
		buffer.size = size;
		buffer.layout = layout;
		glGenBuffers(1, &buffer.id);
		glBindBuffer(GL_ARRAY_BUFFER, buffer.id);
		glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		m_VertexBuffers.push_back(buffer);

		return VertexBufferHandle(m_VertexBuffers.size() - 1);
	}
	IndexBufferHandle OpenGLDevice::CreateIndexBuffer(const uint32_t* data, uint32_t count)
	{
		OpenGL::OpenGLIndexBuffer buffer;
		buffer.count = count;
		glGenBuffers(1, &buffer.id);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer.id);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(uint32_t), data, GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		m_IndexBuffers.push_back(buffer);
		return IndexBufferHandle(m_IndexBuffers.size() - 1);
	}
	VertexArrayHandle OpenGLDevice::CreateVertexArray(VertexBufferHandle vertexHandle, IndexBufferHandle indexHandle)
	{
		OpenGL::OpenGLVertexArray buffer;
		glGenVertexArrays(1, &buffer.id);
		glBindVertexArray(buffer.id);

		glBindBuffer(GL_ARRAY_BUFFER, m_VertexBuffers[vertexHandle.index].id);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IndexBuffers[indexHandle.index].id);

		const auto& layout = m_VertexBuffers[vertexHandle.index].layout;
		for (int i = 0; i < layout.attributes.size(); i++)
		{
			const auto& attrib = layout.attributes[i];
			glEnableVertexAttribArray(i);
			glVertexAttribPointer(
				i,
				attrib.count,
				ShaderDataTypeToOpenGLBaseType(attrib.type),
				attrib.normalized ? GL_TRUE : GL_FALSE,
				layout.stride,
				(const void*)attrib.offset
			);
		}
		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		m_VertexArrays.push_back(buffer);
		return VertexArrayHandle(m_VertexArrays.size() - 1);
	}
	ShaderHandle OpenGLDevice::CreateShader(const char* vertexSrc, const char* fragmentSrc)
	{
		auto compileShader = [](GLenum type, const char* src) -> GLuint
		{
			GLuint shader = glCreateShader(type);
			glShaderSource(shader, 1, &src, nullptr);
			glCompileShader(shader);

			GLint success;
			glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
			if (!success)
			{
				GLint logLength = 0;
				glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);
				std::string infoLog(logLength, '\0');
				glGetShaderInfoLog(shader, logLength, nullptr, infoLog.data());
				const char* shaderType = (type == GL_VERTEX_SHADER) ? "VERTEX" : "FRAGMENT";
				RNX_LOG_ERROR("[OpenGLDevice] ERROR: {} shader compilation failed:\n{}\n", shaderType, infoLog);
				glDeleteShader(shader);
				return 0;
			}

			return shader;
		};

		GLuint vertexShader = compileShader(GL_VERTEX_SHADER, vertexSrc);
		if (!vertexShader) return ShaderHandle();

		GLuint fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentSrc);
		if (!fragmentShader)
		{
			glDeleteShader(vertexShader);
			return ShaderHandle();
		}

		GLuint program = glCreateProgram();
		glAttachShader(program, vertexShader);
		glAttachShader(program, fragmentShader);
		glLinkProgram(program);

		GLint linked;
		glGetProgramiv(program, GL_LINK_STATUS, &linked);
		if (!linked)
		{
			GLint logLength = 0;
			glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLength);
			std::string infoLog(logLength, '\0');
			glGetProgramInfoLog(program, logLength, nullptr, infoLog.data());
			RNX_LOG_ERROR("[OpenGL] ERROR: Shader program linking failed:\n{}\n", infoLog);
			glDeleteProgram(program);
			glDeleteShader(vertexShader);
			glDeleteShader(fragmentShader);
			return ShaderHandle();
		}

		glDetachShader(program, vertexShader);
		glDetachShader(program, fragmentShader);
		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);

		OpenGLShader shader{ program };
		m_shaders.push_back(shader);
		return ShaderHandle(m_shaders.size() - 1);
	}
	void OpenGLDevice::BindVertexBuffer(VertexBufferHandle handle)
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_VertexBuffers[handle.index].id);
	}
	void OpenGLDevice::BindIndexBuffer(IndexBufferHandle handle)
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IndexBuffers[handle.index].id);
	}
	void OpenGLDevice::BindVertexArray(VertexArrayHandle handle)
	{
		glBindVertexArray(m_VertexArrays[handle.index].id);
	}
	void OpenGLDevice::BindShader(ShaderHandle handle)
	{
		glUseProgram(m_shaders[handle.index].id);
	}
	void OpenGLDevice::UnBindVertexBuffer()
	{
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
	void OpenGLDevice::UnBindIndexBuffer()
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
	void OpenGLDevice::UnBindVertexArray()
	{
		glBindVertexArray(0);
	}
	void OpenGLDevice::UnBindShader()
	{
		glUseProgram(0);
	}
}