#include "Platform/ModuleService.h"

#include <Platform/Platform.h>
#include <Common/Logger.h>

namespace Rynox
{
	bool ModuleService::Initialize()
	{
		return true;
	}

	void ModuleService::Shutdown()
	{
		for (auto& [name, info] : m_modules)
		{
			if (info.Instance)
			{
				info.Instance->Shutdown();
			}

			using DestroyFn = void (*)(IModule*);
			auto destroy = reinterpret_cast<DestroyFn>(
				Platform::GetSymbol(info.Library, "DestroyModule")
				);
			if (destroy)
				destroy(info.Instance);

			Platform::UnloadLibrary(info.Library);
		}

		m_modules.clear();
	}

	bool ModuleService::LoadModule(const std::string& path, const std::string& name)
	{
		void* library = Platform::LoadLibrary(path);
		if (!library)
		{
			RNX_LOG_ERROR("Failed to load module from {}", path);
			return false;
		}

		using CreateFn = IModule * (*)();
		auto create = reinterpret_cast<CreateFn>(
			Platform::GetSymbol(library, "CreateModule")
		);
		if (!create)
		{
			RNX_LOG_ERROR("Failed to find CreateModule function in {}", path);
			return false;
		}

		IModule* module = create();
		if (!module)
		{
			RNX_LOG_ERROR("Failed to create module instance from {}", path);
			return false;
		}

		module->Initialize();
		m_modules[name] = { library, module};
		return true;
	}
	bool ModuleService::UnloadModule(const std::string& name)
	{
		auto it = m_modules.find(name);
		if (it == m_modules.end())
			return false;

		if (!it->second.Instance)
			return false;

		it->second.Instance->Shutdown();

		using DestroyFn = void (*)(IModule*);
		auto destroy = reinterpret_cast<DestroyFn>(
			Platform::GetSymbol(it->second.Library, "DestroyModule")
			);

		if (!destroy)
			return false;

		destroy(it->second.Instance);

		Platform::UnloadLibrary(it->second.Library);
		m_modules.erase(it);

		return true;
	}

	IModule* ModuleService::GetModule(const std::string& name)
	{
		auto it = m_modules.find(name);
		if (it != m_modules.end())
		{
			return it->second.Instance;
		}
		return nullptr;
	}
}