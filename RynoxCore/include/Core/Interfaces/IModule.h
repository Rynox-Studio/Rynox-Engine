#pragma once

#include <string>

namespace Rynox::Core {
	class IModule {
	public:
		virtual bool Load() = 0;
		virtual void UnLoad() = 0;
		virtual ~IModule() = default;
	};
}
