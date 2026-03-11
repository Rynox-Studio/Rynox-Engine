#pragma once

#include <cstdint>
#include <vector>

#include <Common/Assert.h>
#include <ECS/Entity.h>

namespace Rynox::ECS
{
	class EntityPool
	{
	public:
		using size_type = EntityID;

        [[nodiscard]] Entity Create()
        {
            if (!m_Freelist.empty())
            {
                EntityID id = m_Freelist.back();
                m_Freelist.pop_back();
                return { id, m_Generations[id] };
            }
            EntityID id = m_Counter++;
            m_Generations.push_back(0);
            return { id, 0 };
        }

        void Destroy(Entity entity)
        {
            if (IsAlive(entity))
            {
                m_Generations[entity.id]++;
                m_Freelist.push_back(entity.id);
            }
        }

        [[nodiscard]] bool IsAlive(Entity entity) const
        {
            return (
                entity.id < m_Generations.size() &&
                m_Generations[entity.id] == entity.gen
                );
        }

        [[nodiscard]] size_type AliveCount() const
        {
            return m_Counter - static_cast<size_type>(m_Freelist.size());
        }

        [[nodiscard]] Entity Get(EntityID id) const
        {
            if (m_Generations.size() > id)
            {
                return { id, m_Generations[id] };
            }
            return NULL_ENTITY;
        }

	private:
		std::vector<EntityGen> m_Generations;
		std::vector<EntityID> m_Freelist;
		size_type m_Counter = 0;
	};
}
