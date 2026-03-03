#pragma once

#include <cstdint>

namespace Rynox::Common
{
	class UUID
	{
	public:
		UUID();
		UUID(uint64_t uuid);
		UUID(const UUID&) = default;

		operator uint64_t() const;

	private:
		uint64_t m_UUID;
	};
}
