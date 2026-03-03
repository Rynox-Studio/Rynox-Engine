#pragma once

#include <cstdint>

namespace Rynox::ECS
{
	using EntityID = uint32_t;
	using EntityGen = uint32_t;

	struct Entity
	{
		EntityID id;
		EntityGen gen;
	};
}
