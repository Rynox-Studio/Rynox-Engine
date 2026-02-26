#pragma once

#include <string>
#include "Renderer/ShaderDataType.h"

namespace Rynox::Renderer
{
	struct BufferElement
	{
	public:
		std::string name;
		ShaderDataType type;
		uint32_t size;
		uint32_t offset;
		bool normalized;

		BufferElement(ShaderDataType type, const std::string& name, bool normalized = false)
			: name(name), type(type), size(ShaderDataTypeSize(type)), offset(0), normalized(normalized)
		{
		}
	};
}