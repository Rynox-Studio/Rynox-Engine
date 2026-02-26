#pragma once

#include <entt/entity/registry.hpp>

namespace Rynox::ECS
{
	using Entity = entt::entity;

	class World
	{
	public:
		World() = default;
		~World() = default;

		Entity CreateEntity();
		void DestroyEntity(Entity e);

		template<typename Type, typename... Args>
		decltype(auto) InsertComponent(Entity e, Args&&... args)
		{
			return m_Registry.emplace_or_replace<Type, Args>(e, std::forward<Args>(args)...);
		}

		template<typename Type, typename... Other>
		void RemoveComponents(Entity e)
		{
			m_Registry.remove<Type, Other>(e);
		}

		template<typename Type, typename... Other, typename... Exclude>
		decltype(auto) View()
		{
			return m_Registry.view<Type, Other, Exclude>();
		}

	private:
		entt::registry m_Registry;
	};
}
