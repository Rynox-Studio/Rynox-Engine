#include "Common/UUID.h"

#include <random>

namespace Rynox::Common
{
	static std::random_device rd{};
	static std::mt19937 gen(rd());
	static std::uniform_int_distribution<uint64_t> dist;

	UUID::UUID() : m_UUID(dist(gen))
	{
	}

	UUID::UUID(uint64_t uuid) : m_UUID(uuid)
	{
	}

	UUID::operator uint64_t() const
	{
		return m_UUID;
	}
}
