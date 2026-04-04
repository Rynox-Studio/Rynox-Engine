#pragma once

namespace Rynox
{
	class IService
	{
	public:
		virtual ~IService() = default;

		virtual bool Initialize() = 0;
		virtual void Shutdown() = 0;
	};
}
