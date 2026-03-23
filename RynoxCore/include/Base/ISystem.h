#pragma once

#include <Event/IEvent.h>

namespace Rynox
{
	class ISystem
	{
	public:
		virtual ~ISystem() = default;

		virtual void OnAttach() {}
		virtual void OnDetach() {}

		virtual void OnUpdate(float dt) {}
		virtual void OnEvent(IEvent& event) {}
	};
}