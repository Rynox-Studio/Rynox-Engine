#pragma once

#include "Event/IEvent.h"

namespace Rynox
{
	class ILayer
	{
	public:
		virtual ~ILayer() = default;

		virtual void OnAttach() {}
		virtual void OnDetach() {}

		virtual void OnUpdate(float dt) {}
		virtual void OnEvent(IEvent& e) {}
	};
}
