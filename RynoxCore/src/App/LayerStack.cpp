#include <App/LayerStack.h>

#include <Common/Assert.h>


namespace Rynox
{
	LayerStack::~LayerStack()
	{
		if (m_Ptr)
		{
			for (uint32_t i = 0; i < m_Capacity; i++)
			{
				DestroyAt(i);
			}

			Traits::deallocate(m_Alloc, m_Ptr, m_Capacity);
			m_Ptr = nullptr;
		}
	}

	LayerStack::value_type& LayerStack::Insert(uint32_t index, value_type layer)
	{
		if (m_Capacity <= index)
		{
			Reallocate(index + 1);
		}

		DestroyAt(index);
		m_Ptr[index] = layer;

		return m_Ptr[index];
	}

	LayerStack::value_type& LayerStack::PushBack(value_type layer)
	{
		return Insert(m_PushIndex++, layer);
	}

	void LayerStack::Remove(uint32_t index)
	{
		if (m_Capacity > index)
		{
			DestroyAt(index);
		}
	}

	LayerStack::value_type LayerStack::Get(uint32_t index)
	{
		RNX_ASSERT(m_Capacity > index, "Index out of range!");
		return m_Ptr[index];
	}

	const LayerStack::value_type LayerStack::Get(uint32_t index) const
	{
		RNX_ASSERT(m_Capacity > index, "Index out of range!");
		return m_Ptr[index];
	}

	void LayerStack::Reallocate(uint32_t desired)
	{
		uint32_t newCapacity = std::max(8u, std::max(m_Capacity * 2u, desired));

		value_type* newPtr = Traits::allocate(m_Alloc, (size_t)newCapacity);
		RNX_ASSERT(newPtr, "Failed to allocate memory!");
		if (m_Ptr)
		{
			std::uninitialized_move_n(m_Ptr, m_Capacity, newPtr);
			Traits::deallocate(m_Alloc, m_Ptr, m_Capacity);
		}
		std::fill(newPtr + m_Capacity, newPtr + newCapacity, nullptr);

		m_Ptr = newPtr;
		m_Capacity = newCapacity;
	}

	void LayerStack::DestroyAt(uint32_t index)
	{
		if (m_Ptr[index] != nullptr)
		{
			delete m_Ptr[index];
			m_Ptr[index] = nullptr;
		}
	}
}
