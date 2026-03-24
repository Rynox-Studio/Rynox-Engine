#pragma once

#include <cstdint>
#include <memory>

#include "ILayer.h"

namespace Rynox
{
	class LayerStack
	{
	public:
		using value_type = ILayer*;

		LayerStack() = default;
		~LayerStack();

		LayerStack(const LayerStack&) = delete;
		LayerStack& operator=(const LayerStack&) = delete;

		value_type& Insert(uint32_t index, value_type layer);
		value_type& PushBack(value_type layer);

		void Remove(uint32_t index);

		value_type Get(uint32_t index);
		const value_type Get(uint32_t index) const;

		value_type* begin() { return m_Ptr; }
		const value_type* begin() const { return m_Ptr; }
		const value_type* cbegin() const { return begin(); }

		value_type* end() { return m_Ptr + m_Capacity; }
		const value_type* end() const { return m_Ptr + m_Capacity; }
		const value_type* cend() const { return end(); }

		uint32_t Capacity() const { return m_Capacity; }
		uint32_t PushIndex() const { return m_PushIndex; }

	private:
		using Alloc = std::allocator<value_type>;
		using Traits = std::allocator_traits<Alloc>;

		void Reallocate(uint32_t desired);
		void DestroyAt(uint32_t index);

	private:
		Alloc m_Alloc;

		value_type* m_Ptr = nullptr;
		uint32_t m_Capacity = 0;
		uint32_t m_PushIndex = 0;
	};
}
