#include "Core/Services/ModuleService.h"

#include <Common/Logger.h>

namespace Rynox::Core::Service
{
	ModuleService::~ModuleService()
	{
		for (auto& [name, info] : m_modules)
		{
			if (info.instance)
			{
				info.instance->Shutdown();
			}

			using DestroyFn = void (*)(IModule*);
			auto destroy = reinterpret_cast<DestroyFn>(
				IO::DynamicLibrary::GetSymbol(info.handle, "DestroyModule")
			);
			if (destroy)
				destroy(info.instance);

			IO::DynamicLibrary::UnLoad(info.handle);
		}

		m_modules.clear();
	}
	bool ModuleService::Initialize() noexcept(true)
	{
		return true;
	}
	bool ModuleService::LoadModule(const std::string& path, const std::string& name)
	{
		LibHandle handle = IO::DynamicLibrary::Load(path);
		if (!handle)
		{
			RNX_LOG_ERROR("Failed to load module from {}", path);
			return false;
		}

		using CreateFn = IModule * (*)();
		auto create = reinterpret_cast<CreateFn>(
			IO::DynamicLibrary::GetSymbol(handle, "CreateModule")
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
		m_modules[name] = {handle, module};
		return true;
	}
	bool ModuleService::UnloadModule(const std::string& name)
	{
		auto it = m_modules.find(name);
		if (it == m_modules.end())
			return false;

		if (!it->second.instance)
			return false;

		it->second.instance->Shutdown();

		using DestroyFn = void (*)(IModule*);
		auto destroy = reinterpret_cast<DestroyFn>(
			IO::DynamicLibrary::GetSymbol(it->second.handle, "DestroyModule")
			);

		if (!destroy)
			return false;

		destroy(it->second.instance);

		if (!IO::DynamicLibrary::UnLoad(it->second.handle))
			return false;

		m_modules.erase(it);

		return true;
	}
	IModule* ModuleService::GetModule(const std::string& name)
	{
		auto it = m_modules.find(name);
		if (it != m_modules.end())
		{
			return it->second.instance;
		}
		return nullptr;
	}
}