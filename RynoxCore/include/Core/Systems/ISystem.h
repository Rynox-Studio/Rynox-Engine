#pragma once

namespace Rynox::Core
{
	class ISystem
	{
	public:
		virtual ~ISystem() = default;

		virtual bool Initialize() noexcept(true) = 0;
		virtual void OnUpdate(float dt) = 0;
	};
}