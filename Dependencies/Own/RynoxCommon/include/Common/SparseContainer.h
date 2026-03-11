#pragma once

#include <cstdint>
#include <utility>
#include <span>

#include <Common/SparseSet.h>

namespace Rynox::Common
{
	template<typename ValueType, typename IDType = uint32_t, typename SizeType = uint32_t>
	class SparseContainer
	{
	public:
		using size_type = SizeType;
		using ID = IDType;
		using value_type = ValueType;

		SparseContainer() = default;

		SparseContainer(const SparseContainer& other)
		{
			m_Sparse = other.m_Sparse;

			if (other.m_Dense && other.m_Container)
			{
				m_Dense = Traits<Dense>::allocate(m_DAlloc, other.m_Capacity);
				RNX_ASSERT(m_Dense, "Failed to allocate memory");

				m_Container = Traits<Container>::allocate(m_CAlloc, other.m_Capacity);
				RNX_ASSERT(m_Container, "Failed to allocate memory");

				std::uninitialized_copy_n(other.m_Dense, other.m_Size, m_Dense);
				std::uninitialized_copy_n(other.m_Container, other.m_Size, m_Container);
				m_Capacity = other.m_Capacity;
				m_Size = other.m_Size;
			}
		}

		SparseContainer(SparseContainer&& other) noexcept
		{
			m_Sparse = std::move(other.m_Sparse);

			if (other.m_Dense && other.m_Container)
			{
				m_Dense = other.m_Dense;
				m_Container = other.m_Container;
				m_Capacity = other.m_Capacity;
				m_Size = other.m_Size;
				other.m_Dense = nullptr;
				other.m_Container = nullptr;
				other.m_Capacity = 0;
				other.m_Size = 0;
			}
		}

		SparseContainer& operator=(const SparseContainer& other)
		{
			if (this != &other)
			{
				m_Sparse = other.m_Sparse;

				if (m_Dense && m_Container)
				{
					Traits<Dense>::deallocate(m_DAlloc, m_Dense, m_Capacity);
					
					for (auto it = m_Container; it != m_Container + m_Size; it++)
					{
						Traits<Container>::destroy(m_CAlloc, it);
					}
					Traits<Container>::deallocate(m_CAlloc, m_Container, m_Capacity);

					m_Dense = nullptr;
					m_Container = nullptr;
					m_Capacity = 0;
					m_Size = 0;
				}

				if (other.m_Dense && other.m_Container)
				{
					m_Dense = Traits<Dense>::allocate(m_DAlloc, other.m_Capacity);
					RNX_ASSERT(m_Dense, "Failed to allocate memory");

					m_Container = Traits<Container>::allocate(m_CAlloc, other.m_Capacity);
					RNX_ASSERT(m_Container, "Failed to allocate memory");

					std::uninitialized_copy_n(other.m_Dense, other.m_Size, m_Dense);
					std::uninitialized_copy_n(other.m_Container, other.m_Size, m_Container);
					m_Capacity = other.m_Capacity;
					m_Size = other.m_Size;
				}
			}
			return *this;
		}

		SparseContainer& operator=(SparseContainer&& other) noexcept
		{
			if (this != &other)
			{
				m_Sparse = std::move(other.m_Sparse);

				if (m_Dense && m_Container)
				{
					Traits<Dense>::deallocate(m_DAlloc, m_Dense, m_Capacity);

					for (auto it = m_Container; it != m_Container + m_Size; it++)
					{
						Traits<Container>::destroy(m_CAlloc, it);
					}
					Traits<Container>::deallocate(m_CAlloc, m_Container, m_Capacity);

					m_Dense = nullptr;
					m_Container = nullptr;
					m_Capacity = 0;
					m_Size = 0;
				}

				if (other.m_Dense && other.m_Container)
				{
					m_Dense = other.m_Dense;
					m_Container = other.m_Container;
					m_Capacity = other.m_Capacity;
					m_Size = other.m_Size;
					other.m_Dense = nullptr;
					other.m_Container = nullptr;
					other.m_Capacity = 0;
					other.m_Size = 0;
				}
			}
			return *this;
		}

		~SparseContainer()
		{
			if (m_Dense && m_Container)
			{
				Traits<Dense>::deallocate(m_DAlloc, m_Dense, m_Capacity);

				for (auto it = m_Container; it != m_Container + m_Size; it++)
				{
					Traits<Container>::destroy(m_CAlloc, it);
				}
				Traits<Container>::deallocate(m_CAlloc, m_Container, m_Capacity);

				m_Dense = nullptr;
				m_Container = nullptr;
				m_Capacity = 0;
				m_Size = 0;
			}
		}
		
		value_type& Insert(ID id, const value_type& element)
		{
			if (Contains(id))
			{
				size_type di = m_Sparse[id];
				m_Container[di] = element;
				return m_Container[di];
			}

			if (m_Size >= m_Capacity)
			{
				Grow(m_Size + 1);
			}

			Traits<Dense>::construct(m_DAlloc, m_Dense + m_Size, id);
			Traits<Container>::construct(m_CAlloc, m_Container + m_Size, element);

			m_Sparse.Insert(id, m_Size);
			return m_Container[m_Size++];
		}

		value_type& Insert(ID id, value_type&& element)
		{
			if (Contains(id))
			{
				size_type di = m_Sparse[id];
				m_Container[di] = std::move(element);
				return m_Container[di];
			}

			if (m_Size >= m_Capacity)
			{
				Grow(m_Size + 1);
			}

			Traits<Dense>::construct(m_DAlloc, m_Dense + m_Size, id);
			Traits<Container>::construct(m_CAlloc, m_Container + m_Size, std::move(element));

			m_Sparse.Insert(id, m_Size);
			return m_Container[m_Size++];
		}

		template<typename... Args>
		value_type& Emplace(ID id, Args&&... args)
		{
			if (Contains(id))
			{
				size_type di = m_Sparse[id];
				Traits<Container>::destroy(m_CAlloc, m_Container + di);
				Traits<Container>::construct(m_CAlloc, m_Container + di, std::forward<Args>(args)...);
				return m_Container[di];
			}

			if (m_Size >= m_Capacity)
			{
				Grow(m_Size + 1);
			}

			Traits<Dense>::construct(m_DAlloc, m_Dense + m_Size, id);
			Traits<Container>::construct(m_CAlloc, m_Container + m_Size, std::forward<Args>(args)...);

			m_Sparse.Insert(id, m_Size);
			return m_Container[m_Size++];
		}

		bool Remove(ID id)
		{
			if (!Contains(id)) return false;

			size_type di = m_Sparse[id];
			size_type last = m_Size - 1;

			// Swap
			if (di != last)
			{
				m_Sparse.Insert(m_Dense[last], di);
				m_Dense[di] = m_Dense[last];
				m_Container[di] = std::move(m_Container[last]);
			}

			// Pop back
			Traits<Dense>::destroy(m_DAlloc, m_Dense + last);
			Traits<Container>::destroy(m_CAlloc, m_Container + last);
			m_Sparse.Remove(id);
			m_Size--;
			return true;
		}

		[[nodiscard]] bool Contains(ID id) const
		{
			return m_Sparse.Contains(id);
		}

		[[nodiscard]] value_type* Get(ID id)
		{
			if (Contains(id))
			{
				return m_Container[m_Sparse[id]];
			}
			return nullptr;
		}

		[[nodiscard]] const value_type* Get(ID id) const
		{
			if (Contains(id))
			{
				return m_Container[m_Sparse[id]];
			}
			return nullptr;
		}

		[[nodiscard]] size_type Size() const { return m_Size; }

		[[nodiscard]] std::span<value_type> Elements() { return { m_Container, m_Size }; }
		[[nodiscard]] std::span<const value_type> Elements() const { return { m_Container, m_Size }; }

		[[nodiscard]] std::span<ID> IDs() { return { m_Dense, m_Size }; }
		[[nodiscard]] std::span<const ID> IDs() const { return { m_Dense, m_Size }; }

	private:
		void Grow(size_type desired)
		{
			size_type newCapacity = std::max(static_cast<size_type>(20), std::max(desired, m_Capacity * static_cast<size_type>(2)));

			Dense* newDense = Traits<Dense>::allocate(m_DAlloc, newCapacity);
			RNX_ASSERT(newDense, "Failed to allocate memory");

			Container* newContainer = Traits<Container>::allocate(m_CAlloc, newCapacity);
			RNX_ASSERT(newContainer, "Failed to allocate memory");

			if (m_Dense && m_Container)
			{
				std::uninitialized_copy_n(m_Dense, m_Size, newDense);
				Traits<Dense>::deallocate(m_DAlloc, m_Dense, m_Capacity);

				std::uninitialized_move_n(m_Container, m_Size, newContainer);
				Traits<Container>::deallocate(m_CAlloc, m_Container, m_Capacity);
			}

			m_Dense = newDense;
			m_Container = newContainer;
			m_Capacity = newCapacity;
		}

	private:
		template<typename T>
		using Alloc = std::allocator<T>;

		template<typename T>
		using Traits = std::allocator_traits<Alloc<T>>;

		using Dense = ID;
		using Container = value_type;

	private:
		SparseSet<ID, size_type> m_Sparse;

		Dense* m_Dense = nullptr;
		Container* m_Container = nullptr;
		size_type m_Capacity = 0;
		size_type m_Size = 0;

		Alloc<Dense> m_DAlloc;
		Alloc<Container> m_CAlloc;
	};
}
