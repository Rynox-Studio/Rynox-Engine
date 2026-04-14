#pragma once

#include <cstdint>
#include <limits>

#include <Common/Assert.h>

namespace Rynox::Common
{
	template<typename IDType = uint32_t, typename SizeType = uint32_t>
	class SparseSet
	{
	public:
		using size_type = SizeType;
		using ID = IDType;

		static constexpr size_type NULL_INDEX = std::numeric_limits<size_type>::max();

		SparseSet() = default;

		SparseSet(const SparseSet& other)
		{
			if (other.m_Sparse)
			{
				m_Sparse = Traits::allocate(m_Alloc, other.m_Capacity);
				RNX_ASSERT(m_Sparse);

				std::uninitialized_copy_n(other.m_Sparse, other.m_Capacity, m_Sparse);
				m_Capacity = other.m_Capacity;
			}
		}

		SparseSet(SparseSet&& other) noexcept
		{
			m_Sparse = other.m_Sparse;
			m_Capacity = other.m_Capacity;
			other.m_Sparse = nullptr;
			other.m_Capacity = 0;
		}

		SparseSet& operator=(const SparseSet& other)
		{
			if (this != &other)
			{
				if (m_Sparse)
				{
					Traits::deallocate(m_Alloc, m_Sparse, m_Capacity);
					m_Sparse = nullptr;
					m_Capacity = 0;
				}

				if (other.m_Sparse)
				{
					m_Sparse = Traits::allocate(m_Alloc, other.m_Capacity);
					RNX_ASSERT(m_Sparse);

					std::uninitialized_copy_n(other.m_Sparse, other.m_Capacity, m_Sparse);
					m_Capacity = other.m_Capacity;
				}
			}
			return *this;
		}

		SparseSet& operator=(SparseSet&& other) noexcept
		{
			if (this != &other)
			{
				if (m_Sparse)
				{
					Traits::deallocate(m_Alloc, m_Sparse, m_Capacity);
				}

				m_Sparse = other.m_Sparse;
				m_Capacity = other.m_Capacity;
				other.m_Sparse = nullptr;
				other.m_Capacity = 0;
			}
			return *this;
		}

		~SparseSet()
		{
			if (m_Sparse)
			{
				Traits::deallocate(m_Alloc, m_Sparse, m_Capacity);
				m_Sparse = nullptr;
				m_Capacity = 0;
			}
		}

		void Insert(ID id, size_type index)
		{
			if (id >= m_Capacity)
			{
				Grow(id + 1);
			}

			m_Sparse[id] = index;
		}

		bool Remove(ID id)
		{
			if (m_Capacity > id)
			{
				m_Sparse[id] = NULL_INDEX;
				return true;
			}
			return false;
		}

		bool Contains(ID id) const
		{
			return (
				m_Capacity > id &&
				m_Sparse[id] != NULL_INDEX
				);
		}

		size_type operator[](ID id)
		{
			if (m_Capacity > id)
			{
				return m_Sparse[id];
			}
			return NULL_INDEX;
		}

		size_type operator[](ID id) const
		{
			if (m_Capacity > id)
			{
				return m_Sparse[id];
			}
			return NULL_INDEX;
		}

		size_type* begin() { return m_Sparse; }
		const size_type* begin() const { return m_Sparse; }
		const size_type* cbegin() const { return m_Sparse; }

		size_type* end() { return m_Sparse + m_Capacity; }
		const size_type* end() const { return m_Sparse + m_Capacity; }
		const size_type* cend() const { return m_Sparse + m_Capacity; }

		size_type Size() const { return m_Capacity; }

	private:
		using Alloc = std::allocator<size_type>;
		using Traits = std::allocator_traits<Alloc>;

		void Grow(size_type desired)
		{
			size_type newCapacity = std::max(static_cast<size_type>(20), std::max(desired, m_Capacity * static_cast<size_type>(2)));

			size_type* newSparse = Traits::allocate(m_Alloc, newCapacity);
			RNX_ASSERT(newSparse);

			if (m_Sparse)
			{
				std::uninitialized_copy_n(m_Sparse, m_Capacity, newSparse);
				Traits::deallocate(m_Alloc, m_Sparse, m_Capacity);
			}

			std::uninitialized_fill_n(newSparse + m_Capacity, newCapacity - m_Capacity, NULL_INDEX);

			m_Sparse = newSparse;
			m_Capacity = newCapacity;
		}

	private:
		size_type* m_Sparse = nullptr;
		size_type m_Capacity = 0;

		Alloc m_Alloc;
	};
}
