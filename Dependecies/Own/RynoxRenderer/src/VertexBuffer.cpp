#include "Renderer/VertexBuffer.h"

namespace Rynox::Renderer
{
	VertexBuffer::VertexBuffer(std::vector<std::byte>&& vertices)
	{
		m_data = std::move(vertices);
	}
	VertexBuffer::VertexBuffer(std::vector<std::byte>&& vertices, BufferLayout layout)
	{
		m_data = std::move(vertices);
		m_layout = layout;
	}
	void VertexBuffer::SetLayout(const BufferLayout& layout)
	{
		m_layout = layout;
	}
	BufferLayout& VertexBuffer::GetLayout()
	{
		return m_layout;
	}
	const BufferLayout& VertexBuffer::GetLayout() const
	{
		return m_layout;
	}
	void* VertexBuffer::GetData()
	{
		return m_data.data();
	}
}
