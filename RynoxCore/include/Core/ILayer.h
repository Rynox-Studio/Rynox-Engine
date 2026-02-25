#pragma once

#include "Events/IEvent.h"

namespace Rynox::Core
{
	class ILayer
	{
	public:
		virtual ~ILayer() = default;

		virtual void OnAttach() {}
		virtual void OnDetach() {}

		virtual void OnUpdate(float dt) {}
		virtual void OnRender() {}

		virtual void OnEvent(IEvent& e) {}
	};
}
