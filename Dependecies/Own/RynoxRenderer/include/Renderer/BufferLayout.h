#pragma once

#include "Renderer/BufferElement.h"
#include <vector>

namespace Rynox::Renderer
{
	struct BufferLayout
	{
	public:
		BufferLayout() : stride(0) {}

		BufferLayout(const std::initializer_list<BufferElement>& elements)
			: attributes(elements)
		{
			CalculateOffsetsAndStride();
		}

		void CalculateOffsetsAndStride()
		{
			uint32_t offset = 0;
			stride = 0;
			for (auto& element : attributes)
			{
				element.offset = offset;
				offset += element.size;
				stride += element.size;
			}
		}
	public:
		std::vector<BufferElement> attributes;
		uint32_t stride;
	};
}