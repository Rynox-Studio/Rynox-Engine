#pragma once

#include <cstdint>

namespace Rynox::Common
{
	class UUID
	{
	public:
		UUID();
		explicit UUID(uint64_t uuid);

		uint64_t GetUUID() const;
		operator uint64_t() const;

	private:
		uint64_t m_UUID;
	};
}
