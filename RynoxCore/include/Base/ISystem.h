#pragma once

#include <Event/IEvent.h>

namespace Rynox
{
	class ISystem
	{
	public:
		virtual ~ISystem() = default;
		virtual void Update(float dt) {}
	};
}