#pragma once

#include <cstdint>
#include <vector>
#include <atomic>
#include <limits>
#include <span>

#include "Core/Interfaces/IService.h"
#include <Common/Assert.h>

namespace Rynox::Core
{
	using AssetID = uint32_t;
	using AssetGen = uint32_t;

	template<typename AssetType>
	struct AssetHandle
	{
		AssetID id;
		AssetGen gen;
	};

	class AssetService final : public IService
	{
	public:
		AssetService()
		{
		}

		~AssetService() override
		{
			for (auto& storage : m_Storages)
			{
				delete storage;
				storage = nullptr;
			}
		}

		bool Initialize() noexcept(true) override
		{
			return true;
		}

		template<typename T>
		AssetHandle<T> CreateAsset(const T& asset)
		{
			if (!HasStorage<T>())
			{
				CreateStorage<T>();
			}

			AssetStorage<T>* storage = GetStorage<T>();
			RNX_ASSERT(storage != nullptr);

			AssetHandle<T> handle;
			if (!m_Free.empty())
			{
				handle.id = m_Free.back();
				m_Free.pop_back();
				handle.gen = m_Generations[handle.id];
			}
			else
			{
				handle.id = m_Generations.size();
				m_Generations.push_back(0);
				handle.gen = m_Generations[handle.id];
			}
			storage->Insert(handle.id, asset);
			return handle;
		}

		template<typename T>
		AssetHandle<T> CreateAsset(T&& asset)
		{
			if (!HasStorage<T>())
			{
				CreateStorage<T>();
			}

			AssetStorage<T>* storage = GetStorage<T>();
			RNX_ASSERT(storage != nullptr);

			AssetHandle<T> handle;
			if (!m_Free.empty())
			{
				handle.id = m_Free.back();
				m_Free.pop_back();
				handle.gen = m_Generations[handle.id]++;
			}
			else
			{
				handle.id = m_Generations.size();
				m_Generations.push_back(0);
				handle.gen = m_Generations[handle.id];
			}
			storage->Insert(handle.id, asset);
			return handle;
		}

		template<typename T>
		void Remove(AssetHandle<T> handle)
		{
			if (Contains(handle))
			{
				AssetStorage<T>* storage = GetStorage<T>();
				storage->Remove(handle.id);
				m_Generations[handle.id];
			}
		}

		template<typename T>
		bool Contains(AssetHandle<T> handle) const
		{
			if (HasStorage<T>() &&
				handle.id < m_Generations.size() &&
				handle.gen == m_Generations[handle.id] &&
				HasStorage<T>()
				)
			{
				AssetStorage<T>* storage = GetStorage<T>();
				return storage->Contains(handle.id);
			}
			return false;
		}

		template<typename T>
		T* GetAsset(AssetHandle<T> handle)
		{
			if (Contains(handle))
			{
				AssetStorage<T>* storage = GetStorage<T>();
				return storage->Get(handle.id);
			}
			return nullptr;
		}

		template<typename T>
		const T* GetAsset(AssetHandle<T> handle) const
		{
			if (Contains(handle))
			{
				AssetStorage<T>* storage = GetStorage<T>();
				return storage->Get(handle.id);
			}
			return nullptr;
		}

		void Clear()
		{
			for (auto* storage : m_Storages)
			{
				if (storage)
				{
					storage->Clear();
				}
			}

			m_Free.clear();
			m_Generations.clear();
		}

	private:
		static uint32_t GetNextID()
		{
			static std::atomic<uint32_t> Counter = 0;
			return Counter++;
		}

		template<typename T>
		static uint32_t GetStaticID()
		{
			static uint32_t ID = GetNextID();
			return ID;
		}

		class IAssetStorage
		{
		public:
			virtual ~IAssetStorage() = default;

			IAssetStorage(const IAssetStorage&) = delete;
			IAssetStorage(IAssetStorage&&) noexcept = delete;

			IAssetStorage& operator=(const IAssetStorage&) = delete;
			IAssetStorage& operator=(IAssetStorage&&) noexcept = delete;

			virtual void Remove(AssetID id) = 0;
			virtual bool Contains(AssetID id) const = 0;
			virtual void Clear() = 0;
		};

		template<typename T>
		class AssetStorage final : public IAssetStorage
		{
		public:
			using value_type = T;
			using size_type = uint32_t;

			AssetStorage()
			{
			}

			~AssetStorage() override
			{
				if (m_Dense)
				{
					Traits<AssetID>::deallocate(m_DenseAlloc, m_Dense, m_DenseCapacity);
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

			void Insert(AssetID id, const value_type& asset)
			{
				if (id >= m_SparseCapacity)
				{
					GrowSparse(id + 1);
				}

				if (Contains(id))
				{
					m_Storage[m_Sparse[id]] = asset;
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
					Traits<value_type>::construct(m_StorageAlloc, m_Storage + m_DenseSize, asset);
					m_DenseSize++;
					m_StorageSize++;
				}
			}

			void Insert(AssetID id, value_type&& asset)
			{
				if (id >= m_SparseCapacity)
				{
					GrowSparse(id + 1);
				}

				if (Contains(id))
				{
					m_Storage[m_Sparse[id]] = std::move(asset);
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
					Traits<value_type>::construct(m_StorageAlloc, m_Storage + m_DenseSize, asset);
					m_DenseSize++;
					m_StorageSize++;
				}
			}

			void Remove(AssetID id)
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

			bool Contains(AssetID id) const
			{
				return (
					id < m_SparseCapacity &&
					m_Sparse[id] != NULL_INDEX &&
					m_Sparse[id] < m_DenseSize &&
					m_Dense[m_Sparse[id]] == id
					);
			}

			value_type* Get(AssetID id)
			{
				if (Contains(id))
				{
					return &m_Storage[m_Sparse[id]];
				}
				return nullptr;
			}

			const value_type* Get(AssetID id) const
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

			std::span<AssetID> GetDenseSpan() { return { m_Dense, m_DenseSize }; }
			std::span<const AssetID> GetDenseSpan() const { return { m_Dense, m_DenseSize }; }

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

				AssetID* newDense = Traits<AssetID>::allocate(m_DenseAlloc, newCapacity);
				if (m_Dense)
				{
					std::uninitialized_copy_n(m_Dense, m_DenseSize, newDense);
					Traits<AssetID>::deallocate(m_DenseAlloc, m_Dense, m_DenseCapacity);
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
			Alloc<AssetID> m_DenseAlloc;
			AssetID* m_Dense = nullptr;
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

		template<typename T>
		AssetStorage<T> CreateStorage()
		{
			auto index = GetStaticID<T>();
			m_Storages.emplace(index);
			RNX_ASSERT(m_Storages.size() >= index);
			RNX_ASSERT(m_Storages[index] != nullptr);
			return m_Storages[index];
		}

		template<typename T>
		void DestroyStorage()
		{
			if (HasStorage<T>())
			{
				auto index = GetStaticID<T>();
				std::destroy_at(m_Storages + index);
				m_Storages[index] = nullptr;
			}
		}

		template<typename T>
		bool HasStorage()
		{
			auto index = GetStaticID<T>();
			return (index < m_Storages.size() && m_Storages[index] != nullptr);
		}

		template<typename T>
		AssetStorage<T>* GetStorage()
		{
			if (HasStorage<T>())
			{
				auto index = GetStaticID<T>();
				return static_cast<AssetStorage<T>*>(m_Storages[index]);
			}
			return nullptr;
		}

	private:
		std::vector<IAssetStorage*> m_Storages;

		std::vector<AssetID> m_Free;
		std::vector<AssetGen> m_Generations;
	};
}
