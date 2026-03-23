#pragma once

namespace Rynox 
{
	class IModule 
	{
	public:
		virtual ~IModule() = default;

		virtual bool Initialize() = 0;
		virtual void Shutdown() = 0;

		virtual const char* GetName() const = 0;
		virtual const char* GetVersion() const = 0;
	};
}
