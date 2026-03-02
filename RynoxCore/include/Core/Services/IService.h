#pragma once

namespace Rynox::Core
{
	class IService
	{
	public:
		virtual ~IService() = default;

		virtual bool Initialize() noexcept(true) = 0;
	};
}
