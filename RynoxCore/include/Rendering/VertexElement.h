#pragma once

#include <string>
#include "ShaderDataType.h"

namespace Rynox
{
	struct VertexElement
	{
	public:
		std::string name;
		ShaderDataType type;
		uint32_t size;
		uint32_t count; // component count
		uint32_t offset;
		bool normalized;

		VertexElement(ShaderDataType type, const std::string& name, bool normalized = false)
			: name(name), type(type),
			size(ShaderDataTypeSize(type)), count(ShaderDataTypeComponentCount(type)),
			offset(0), normalized(normalized)
		{
		}
	};
}