#pragma once

#include <cstdint>
#include <vector>
#include <atomic>

#include "IService.h"
#include <Common/SparseSet.h>
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
				if (m_Storage)
				{
					std::destroy_n(m_Storage, m_Size);
					Traits::deallocate(m_Alloc, m_Storage, m_Capacity);
					m_Storage = nullptr;
					m_Size = 0;
					m_Capacity = 0;
				}
			}

			void Insert(AssetID id, const T& asset)
			{
				auto index = m_SparseSet.Insert(id);
				if (index >= m_Capacity)
				{
					Grow(index);
				}

				m_Storage[index] = asset;
				m_Size++;
			}

			void Insert(AssetID id, T&& asset)
			{
				auto index = m_SparseSet.Insert(id);
				if (index >= m_Capacity)
				{
					Grow(index);
				}

				m_Storage[index] = asset;
				m_Size++;
			}

			void Remove(AssetID id) override
			{
				if (Contains(id))
				{
					auto index = m_SparseSet.GetIndex(id);
					auto last_index = m_Size - 1;
					std::swap(m_Storage[index], m_Storage[last_index]);
					std::destroy_at(m_Storage[last_index]);
					m_Size--;

					m_SparseSet.Remove(id);
				}
			}

			bool Contains(AssetID id) const override
			{
				return m_SparseSet.Contains(id);
			}

			T* Get(AssetID id)
			{
				if (Contains(id))
				{
					return &m_Storage[m_SparseSet.GetIndex(id)];
				}
				return nullptr;
			}

			const T* Get(AssetID id) const
			{
				if (Contains(id))
				{
					return &m_Storage[m_SparseSet.GetIndex(id)];
				}
				return nullptr;
			}

			void Clear() override
			{
				if (m_Storage)
				{
					std::destroy_n(m_Storage, m_Size);
					Traits::deallocate(m_Alloc, m_Storage, m_Capacity);
				}
				m_SparseSet.Clear();
			}

			value_type* begin() { return m_Storage; }
			const value_type* begin() const { return m_Storage; }
			const value_type* cbegin() const { return m_Storage; }

			value_type* end() { return m_Storage + m_Size; }
			const value_type* end() const { return m_Storage + m_Size; }
			const value_type* cend() const { return m_Storage + m_Size; }

		private:
			using Alloc = std::allocator<value_type>;
			using Traits = std::allocator_traits<Alloc>;

			void Reallocate(size_type newCapacity)
			{
				if (m_Capacity >= newCapacity) return;

				value_type* newStorage = Traits::allocate(m_Alloc, newCapacity);
				if (m_Storage)
				{
					std::uninitialized_move_n(m_Storage, m_Size, newStorage);
					Traits::deallocate(m_Alloc, m_Storage, m_Capacity);
				}
				m_Storage = newStorage;
				m_Capacity = newCapacity;
			}

			void Grow(size_type desired)
			{
				Reallocate(m_Capacity == 0 ? 8 : (m_Capacity * 2 > desired ? m_Capacity * 2 : desired));
			}

		private:
			Common::SparseSet m_SparseSet;

			value_type* m_Storage = nullptr;
			size_type m_Size = 0;
			size_type m_Capacity = 0;

			Alloc m_Alloc;
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
				std::destroy(m_Storages[index]);
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
			auto index = GetStaticID<T>();
			RNX_ASSERT(m_Storages[index] != nullptr);
			return static_cast<AssetStorage<T>*>(m_Storages[index]);
		}

	private:
		std::vector<IAssetStorage*> m_Storages;

		std::vector<AssetID> m_Free;
		std::vector<AssetGen> m_Generations;
	};
}
