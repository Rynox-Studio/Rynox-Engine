#pragma once

namespace Rynox
{
	class IService
	{
	public:
		virtual ~IService() = default;

		virtual bool Initialize() {}
		virtual void Shutdown() {}
	};
}
