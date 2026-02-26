#include "ECS/World.h"

namespace Rynox::ECS
{
	Entity World::CreateEntity()
	{
		return m_Registry.create();
	}

	void World::DestroyEntity(Entity e)
	{
		m_Registry.destroy(e);
	}
}
