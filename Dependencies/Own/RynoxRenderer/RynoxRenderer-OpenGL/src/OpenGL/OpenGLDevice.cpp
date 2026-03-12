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
	OpenGLVertexBuffer OpenGLDevice::CreateVertexBuffer(const void* data, uint32_t size, Graphics::VertexLayout& layout)
	{
		OpenGL::OpenGLVertexBuffer buffer;
		buffer.size = size;
		buffer.layout = layout;
		glGenBuffers(1, &buffer.id);
		glBindBuffer(GL_ARRAY_BUFFER, buffer.id);
		glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		return buffer;
	}
	OpenGLIndexBuffer OpenGLDevice::CreateIndexBuffer(const uint32_t* data, uint32_t count)
	{
		OpenGL::OpenGLIndexBuffer buffer;
		buffer.count = count;
		glGenBuffers(1, &buffer.id);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer.id);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(uint32_t), data, GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		return buffer;
	}
	OpenGLVertexArray OpenGLDevice::CreateVertexArray(const OpenGLVertexBuffer& vertexBuffer, const OpenGLIndexBuffer& indexBuffer)
	{
		OpenGL::OpenGLVertexArray array;
		glGenVertexArrays(1, &array.id);
		glBindVertexArray(array.id);

		glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer.id);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer.id);

		const auto& layout = vertexBuffer.layout;
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
				reinterpret_cast<void*>(static_cast<uintptr_t>(attrib.offset))
			);
		}
		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		return array;
	}
	OpenGLShader OpenGLDevice::CreateShader(const char* vertexSrc, const char* fragmentSrc)
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
		if (!vertexShader) return OpenGLShader{};

		GLuint fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentSrc);
		if (!fragmentShader)
		{
			glDeleteShader(vertexShader);
			return OpenGLShader{};
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
			return OpenGLShader{};
		}

		glDetachShader(program, vertexShader);
		glDetachShader(program, fragmentShader);
		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);

		OpenGLShader shader{ program };

		return shader;
	}
	void OpenGLDevice::BindVertexBuffer(const OpenGLVertexBuffer& buffer)
	{
		glBindBuffer(GL_ARRAY_BUFFER, buffer.id);
	}
	void OpenGLDevice::BindIndexBuffer(const OpenGLIndexBuffer& buffer)
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer.id);
	}
	void OpenGLDevice::BindVertexArray(const OpenGLVertexArray& array)
	{
		glBindVertexArray(array.id);
	}
	void OpenGLDevice::BindShader(const OpenGLShader& shader)
	{
		glUseProgram(shader.id);
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