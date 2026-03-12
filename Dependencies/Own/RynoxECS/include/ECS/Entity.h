#pragma once

#include <cstdint>
#include <limits>

namespace Rynox::ECS
{
	using EntityID = uint32_t;
	using EntityGen = uint32_t;

    struct Entity
    {
        EntityID id;
        EntityGen gen;

        bool operator==(const Entity& other) const { return id == other.id && gen == other.gen; }
        bool operator!=(const Entity& other) const { return !(*this == other); }
    };

    inline constexpr Entity NULL_ENTITY = { std::numeric_limits<EntityID>::max(), 0 };
}
