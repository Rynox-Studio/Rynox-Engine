#pragma once

#include <string>

namespace Rynox {
	class IModule {
	public:
		virtual ~IModule() = default;
		virtual bool Initialize() = 0;
		virtual void Shutdown() = 0;
		virtual std::string GetName() const = 0;
		virtual std::string GetVersion() const = 0;
	};
}
