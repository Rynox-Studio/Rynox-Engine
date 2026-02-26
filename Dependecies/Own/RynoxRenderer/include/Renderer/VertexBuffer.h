#pragma once

#include <vector>
#include "Renderer/BufferLayout.h"

namespace Rynox::Renderer
{
	struct VertexBuffer
	{
	public:
		VertexBuffer(std::vector<std::byte>&& vertices);
		VertexBuffer(std::vector<std::byte>&& vertices, BufferLayout layout);
		void SetLayout(const BufferLayout& layout);
		BufferLayout& GetLayout();
		const BufferLayout& GetLayout() const;
		void* GetData();
	private:
		std::vector<std::byte> m_data;
		BufferLayout m_layout;
	};
}