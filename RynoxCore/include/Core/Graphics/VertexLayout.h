#pragma once

#include "Core/Graphics/VertexElement.h"
#include <vector>

namespace Rynox::Graphics
{
	struct VertexLayout
	{
	public:
		VertexLayout() : stride(0) {}

		VertexLayout(const std::initializer_list<VertexElement>& elements)
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
		std::vector<VertexElement> attributes;
		uint32_t stride;
	};
}