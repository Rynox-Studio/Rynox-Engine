#pragma once

#include <ECS/EntityPool.h>
#include <ECS/ComponentStorage.h>
#include <ECS/TypeID.h>

namespace Rynox::ECS
{
	class World
	{
	public:
		using size_type = uint32_t;

		~World()
		{
			for (auto* storage : m_Storages)
			{
				delete storage;
			}
		}

		[[nodiscard]] Entity CreateEntity()
		{
			return m_EntityPool.Create();
		}

		void DestroyEntity(Entity entity)
		{
			for (auto* storage : m_Storages)
			{
				if (storage)
				{
					storage->Remove(entity.id);
				}
			}
			m_EntityPool.Destroy(entity);
		}

		[[nodiscard]] bool IsAlive(Entity entity) const
		{
			return m_EntityPool.IsAlive(entity);
		}

		template<typename T, typename... Args>
		T* Emplace(Entity entity, Args&&... args)
		{
			if (!IsAlive(entity))
			{
				return nullptr;
			}

			CreateStorage<T>();
			return GetStorage<T>().Emplace(entity.id, std::forward<Args>(args)...);
		}

		template<typename T>
		void Remove(Entity entity)
		{
			if (IsAlive(entity) && HasStorage<T>())
			{
				GetStorage<T>().Remove(entity.id);
			}
		}

		template<typename T>
		[[nodiscard]] T* Get(Entity entity)
		{
			if (!IsAlive(entity) || !HasStorage<T>())
			{
				return nullptr;
			}
			return GetStorage<T>().Get(entity.id);
		}

		template<typename T>
		[[nodiscard]] const T* Get(Entity entity) const
		{
			if (!IsAlive(entity) || !HasStorage<T>())
			{
				return nullptr;
			}
			return GetStorage<T>().Get(entity.id);
		}

		template<typename T>
		[[nodiscard]] bool Contains(Entity entity) const
		{
			if (!IsAlive(entity) || !HasStorage<T>())
			{
				return false;
			}
			return GetStorage<T>()->Contains(entity.id);
		}

	private:
		template<typename T>
		void CreateStorage()
		{
			size_type index = TypeID::Get<T>();
			if (index >= m_Storages.size())
			{
				m_Storages.resize(index + 1, nullptr);
			}
			if (m_Storages[index] == nullptr)
			{
				m_Storages[index] = new ComponentStorage<T>();
				RNX_ASSERT(m_Storages[index], "Failed to allocate memory");
			}
		}

		template<typename T>
		void DestroyStorage()
		{
			size_type index = TypeID::Get<T>();
			if (index < m_Storages.size() && m_Storages[index])
			{
				delete m_Storages[index];
				m_Storages[index] = nullptr;
			}
		}

		template<typename T>
		ComponentStorage<T>* GetStorage()
		{
			size_type index = TypeID::Get<T>();
			if (index < m_Storages.size() && m_Storages[index])
			{
				return static_cast<ComponentStorage<T>*>(m_Storages[index]);
			}
			return nullptr;
		}

		template<typename T>
		const ComponentStorage<T>* GetStorage() const
		{
			size_type index = TypeID::Get<T>();
			if (index < m_Storages.size() && m_Storages[index])
			{
				return static_cast<ComponentStorage<T>*>(m_Storages[index]);
			}
			return nullptr;
		}

		template<typename T>
		bool HasStorage() const
		{
			size_type index = TypeID::Get<T>();
			return (
				index < m_Storages.size() &&
				m_Storages[index] != nullptr
				);
		}

	private:
		EntityPool m_EntityPool;
		std::vector<IComponentStorage*> m_Storages;
	};
}
