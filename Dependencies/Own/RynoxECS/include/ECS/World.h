#pragma once

#include <atomic>
#include <vector>

#include <Common/Assert.h>

#include "ComponentStorage.h"

namespace Rynox::ECS
{
	class World
	{
	public:
		World()
		{
		}

		~World()
		{
			for (auto& storage : m_Storages)
			{
				delete storage;
				storage = nullptr;
			}
		}

		Entity CreateEntity()
		{
			Entity e;
			if (!m_Free.empty())
			{
				e.id = m_Free.back();
				e.gen = m_Generations[e.id];
				m_Free.pop_back();
			}
			else
			{
				e.id = m_Generations.size();
				e.gen = 0;
				m_Generations.push_back(0);
			}
			return e;
		}

		void DestroyEntity(Entity e)
		{
			if (HasEntity(e))
			{
				m_Free.push_back(e.id);
				m_Generations[e.id]++;

				for (auto& storage : m_Storages)
				{
					storage->Remove(e.id);
				}
			}
		}

		bool HasEntity(Entity e) const
		{
			return (
				e.id < m_Generations.size() &&
				e.gen == m_Generations[e.id]
				);
		}

		template<typename T>
		T& SetComponent(Entity e, const T& component)
		{
			RNX_ASSERT(HasEntity(e), "Invalid entity");
			if (!HasStorage<T>())
			{
				CreateStorage<T>();
			}
			ComponentStorage<T>* storage = GetStorage<T>();
			storage->Insert(e.id, component);
			return *storage->Get(e.id);
		}

		template<typename T>
		T& SetComponent(Entity e, T&& component)
		{
			RNX_ASSERT(HasEntity(e), "Invalid entity");
			if (!HasStorage<T>())
			{
				CreateStorage<T>();
			}
			ComponentStorage<T>* storage = GetStorage<T>();
			storage->Insert(e.id, std::move(component));
			return *storage->Get(e.id);
		}

		template<typename T>
		void RemoveComponent(Entity e)
		{
			RNX_ASSERT(HasEntity(e), "Invalid entity");
			ComponentStorage<T>* storage = GetStorage<T>();
			if (storage)
			{
				storage->Remove(e.id);
			}
		}

		template<typename T>
		bool HasComponent(Entity e)
		{
			RNX_ASSERT(HasEntity(e), "Invalid entity");
			ComponentStorage<T>* storage = GetStorage<T>();
			if (storage)
			{
				return storage->Contains(e.id);
			}
			return false;
		}

		template<typename T>
		T* GetComponent(Entity e)
		{
			RNX_ASSERT(HasEntity(e), "Invalid entity");
			ComponentStorage<T>* storage = GetStorage<T>();
			if (storage)
			{
				return storage->Get(e.id);
			}
			return nullptr;
		}

		template<typename T>
		const T* GetComponent(Entity e)
		{
			RNX_ASSERT(HasEntity(e), "Invalid entity");
			ComponentStorage<T>* storage = GetStorage<T>();
			if (storage)
			{
				return storage->Get(e.id);
			}
			return nullptr;
		}

	private:
		static uint32_t GetNextIndex()
		{
			static std::atomic<uint32_t> Counter = 0;
			return Counter++;
		}

		template<typename T>
		static uint32_t GetStaticIndex()
		{
			static uint32_t Index = GetNextIndex();
			return Index;
		}

		template<typename T>
		void CreateStorage()
		{
			if (!HasStorage<T>())
			{
				m_Storages.emplace(GetStaticIndex<T>(), new ComponentStorage<T>());
			}
		}

		template<typename T>
		void DestroyStorage()
		{
			if (HasStorage<T>())
			{
				uint32_t index = GetStaticIndex<T>();
				std::destroy_at(&m_Storages.data() + index);
				m_Storages[index] = nullptr;
			}
		}

		template<typename T>
		bool HasStorage() const
		{
			uint32_t index = GetStaticIndex<T>();
			return (
				index < m_Storages.size() &&
				m_Storages[index] != nullptr
				);
		}

		template<typename T>
		ComponentStorage<T>* GetStorage()
		{
			if (HasStorage<T>())
			{
				return &m_Storages[GetStaticIndex<T>()];
			}
			return nullptr;
		}

		template<typename T>
		const ComponentStorage<T>* GetStorage() const
		{
			if (HasStorage<T>())
			{
				return &m_Storages[GetStaticIndex<T>()];
			}
			return nullptr;
		}

	private:
		std::vector<IComponentStorage*> m_Storages;

		std::vector<EntityID> m_Free;
		std::vector<EntityGen> m_Generations;
	};
}
