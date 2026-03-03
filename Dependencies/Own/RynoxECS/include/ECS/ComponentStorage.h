#pragma once

#include <utility>
#include <span>

#include <Common/Assert.h>

#include "Entity.h"

namespace Rynox::ECS
{
	class IComponentStorage
	{
	public:
		virtual ~IComponentStorage() = default;

		IComponentStorage(const IComponentStorage&) = delete;
		IComponentStorage(IComponentStorage&&) noexcept = delete;

		IComponentStorage& operator=(const IComponentStorage&) = delete;
		IComponentStorage& operator=(IComponentStorage&&) noexcept = delete;

		virtual void Remove(EntityID id) = 0;
		virtual bool Contains(EntityID id) const = 0;
		virtual void Clear() = 0;
	};

	template<typename T>
	class ComponentStorage final : public IComponentStorage
	{
	public:
		using value_type = T;
		using size_type = uint32_t;

		ComponentStorage()
		{
		}

		~ComponentStorage() override
		{
			if (m_Dense)
			{
				Traits<EntityID>::deallocate(m_DenseAlloc, m_Dense, m_DenseCapacity);
				m_Dense = nullptr;
				m_DenseCapacity = 0;
				m_DenseSize = 0;
			}

			if (m_Sparse)
			{
				Traits<size_type>::deallocate(m_SparseAlloc, m_Sparse, m_SparseCapacity);
				m_Sparse = nullptr;
				m_SparseCapacity = 0;
				m_SparseSize = 0;
			}

			if (m_Storage)
			{
				for (size_type i = 0; i < m_StorageSize; i++)
				{
					Traits<value_type>::destroy(m_StorageAlloc, m_Storage + i);
				}

				Traits<size_type>::deallocate(m_StorageAlloc, m_Storage, m_StorageCapacity);
				m_Storage = nullptr;
				m_StorageCapacity = 0;
				m_StorageSize = 0;
			}
		}

		void Insert(EntityID id, const value_type& component)
		{
			if (id >= m_SparseCapacity)
			{
				GrowSparse(id + 1);
			}

			if (Contains(id))
			{
				m_Storage[m_Sparse[id]] = component;
			}
			else
			{
				if (m_DenseSize >= m_DenseCapacity)
				{
					GrowDense(0);
				}
				if (m_StorageSize >= m_StorageCapacity)
				{
					GrowStorage(0);
				}

				m_Dense[m_DenseSize] = id;
				m_Sparse[id] = m_DenseSize;
				Traits<value_type>::construct(m_StorageAlloc, m_Storage + m_DenseSize, component);
				m_DenseSize++;
				m_StorageSize++;
			}
		}

		void Insert(EntityID id, value_type&& component)
		{
			if (id >= m_SparseCapacity)
			{
				GrowSparse(id + 1);
			}

			if (Contains(id))
			{
				m_Storage[m_Sparse[id]] = std::move(component);
			}
			else
			{
				if (m_DenseSize >= m_DenseCapacity)
				{
					GrowDense(0);
				}
				if (m_StorageSize >= m_StorageCapacity)
				{
					GrowStorage(0);
				}

				m_Dense[m_DenseSize] = id;
				m_Sparse[id] = m_DenseSize;
				Traits<value_type>::construct(m_StorageAlloc, m_Storage + m_DenseSize, component);
				m_DenseSize++;
				m_StorageSize++;
			}
		}

		void Remove(EntityID id)
		{
			if (Contains(id))
			{
				size_type di = m_Sparse[id];
				size_type last = m_DenseSize - 1;
				if (di != last)
				{
					m_Dense[di] = m_Dense[last];
					m_Storage[di] = std::move(m_Storage[last]);
					m_Sparse[m_Dense[di]] = di;
				}

				Traits<value_type>::destroy(m_StorageAlloc, m_Storage + last);
				m_Sparse[id] = NULL_INDEX;
				m_DenseSize--;
				m_StorageSize--;
			}
		}

		bool Contains(EntityID id) const
		{
			return (
				id < m_SparseCapacity &&
				m_Sparse[id] != NULL_INDEX &&
				m_Sparse[id] < m_DenseSize &&
				m_Dense[m_Sparse[id]] == id
				);
		}

		value_type* Get(EntityID id)
		{
			if (Contains(id))
			{
				return &m_Storage[m_Sparse[id]];
			}
			return nullptr;
		}

		const value_type* Get(EntityID id) const
		{
			if (Contains(id))
			{
				return &m_Storage[m_Sparse[id]];
			}
			return nullptr;
		}

		void Clear()
		{
			if (m_Dense)
			{
				m_DenseSize = 0;
			}

			if (m_Sparse)
			{
				std::fill_n(m_Sparse, m_SparseCapacity, NULL_INDEX);
			}

			if (m_Storage)
			{
				for (size_type i = 0; i < m_StorageSize; i++)
				{
					Traits<value_type>::destroy(m_StorageAlloc, m_Storage + i);
				}

				m_StorageSize = 0;
			}
		}

		std::span<EntityID> GetDenseSpan() { return { m_Dense, m_DenseSize }; }
		std::span<const EntityID> GetDenseSpan() const { return { m_Dense, m_DenseSize }; }

		std::span<value_type> GetStorageSpan() { return { m_Storage, m_StorageSize }; }
		std::span<const value_type> GetStorageSpan() const { return { m_Storage, m_StorageSize }; }

	private:
		template<typename T>
		using Alloc = std::allocator<T>;

		template<typename T>
		using Traits = std::allocator_traits<Alloc<T>>;

		static constexpr size_type NULL_INDEX = std::numeric_limits::max();

		void ReallocateDense(size_type newCapacity)
		{
			if (newCapacity <= m_DenseCapacity) return;

			EntityID* newDense = Traits<EntityID>::allocate(m_DenseAlloc, newCapacity);
			if (m_Dense)
			{
				std::uninitialized_copy_n(m_Dense, m_DenseSize, newDense);
				Traits<EntityID>::deallocate(m_DenseAlloc, m_Dense, m_DenseCapacity);
			}

			m_Dense = newDense;
			m_DenseCapacity = newCapacity;
		}

		void ReallocateSparse(size_type newCapacity)
		{
			if (newCapacity <= m_SparseCapacity) return;

			size_type* newSparse = Traits<size_type>::allocate(m_SparseAlloc, newCapacity);
			if (m_Sparse)
			{
				std::uninitialized_copy_n(m_Sparse, m_SparseCapacity, newSparse);
				Traits<size_type>::deallocate(m_SparseAlloc, m_Sparse, m_SparseCapacity);
			}

			std::fill_n(newSparse + m_SparseCapacity, newCapacity - m_SparseCapacity, NULL_INDEX);

			m_Sparse = newSparse;
			m_SparseCapacity = newCapacity;
		}

		void ReallocateStorage(size_type newCapacity)
		{
			if (newCapacity <= m_StorageCapacity) return;

			value_type* newStorage = Traits<value_type>::allocate(m_StorageAlloc, newCapacity);
			if (m_Storage)
			{
				std::uninitialized_move_n(m_Storage, m_StorageSize, newStorage);
				Traits<value_type>::deallocate(m_StorageAlloc, m_Storage, m_StorageCapacity);
			}

			m_Storage = newStorage;
			m_StorageCapacity = newCapacity;
		}

		void GrowDense(size_type desired)
		{
			size_type cap = std::max(8u, std::max(m_DenseCapacity * 2, desired));
			ReallocateDense(cap);
		}

		void GrowSparse(size_type desired)
		{
			size_type cap = std::max(8u, std::max(m_SparseCapacity * 2, desired));
			ReallocateSparse(cap);
		}

		void GrowStorage(size_type desired)
		{
			size_type cap = std::max(8u, std::max(m_StorageCapacity * 2, desired));
			ReallocateStorage(cap);
		}

	private:
		Alloc<EntityID> m_DenseAlloc;
		EntityID* m_Dense = nullptr;
		size_type m_DenseCapacity = 0;
		size_type m_DenseSize = 0;

		Alloc<size_type> m_SparseAlloc;
		size_type* m_Sparse = nullptr;
		size_type m_SparseCapacity = 0;

		Alloc<value_type> m_StorageAlloc;
		value_type* m_Storage = nullptr;
		size_type m_StorageCapacity = 0;
		size_type m_StorageSize = 0;
	};
}
