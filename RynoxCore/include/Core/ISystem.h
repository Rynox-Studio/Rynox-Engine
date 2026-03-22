#pragma once

namespace Rynox
{
	class ISystem
	{
	public:
		virtual ~ISystem() = default;

		virtual bool Initialize() noexcept(true) = 0;
		virtual void OnUpdate(float dt, float time) = 0;
	};
}