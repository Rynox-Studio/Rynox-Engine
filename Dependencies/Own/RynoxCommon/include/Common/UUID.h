#pragma once

#include <cstdint>
#include <type_traits>

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

namespace std
{
	template<>
	struct hash<Rynox::Common::UUID>
	{
		size_t operator()(const Rynox::Common::UUID& uuid) const noexcept
		{
			return (uint64_t)uuid;
		}
	};
}
