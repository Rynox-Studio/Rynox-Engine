#include "Core/Services/ModuleService.h"

namespace Rynox::Core
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
	bool ModuleService::LoadModule(const std::string& path)
	{
		LibHandle handle = IO::DynamicLibrary::Load(path);
		if (!handle) return false;

		using CreateFn = IModule * (*)();
		auto create = reinterpret_cast<CreateFn>(
			IO::DynamicLibrary::GetSymbol(handle, "CreateModule")
		);
		if (!create) return false;

		IModule* module = create();
		if (!module) return false;

		module->Initialize();
		m_modules[module->GetName()] = {handle, module};
		return true;
	}
	void ModuleService::UnloadModule(const std::string& name)
	{
		auto it = m_modules.find(name);
		if (it == m_modules.end()) return;

		it->second.instance->Shutdown();

		using DestroyFn = void (*)(IModule*);
		auto destroy = reinterpret_cast<DestroyFn>(
			IO::DynamicLibrary::GetSymbol(it->second.handle, "DestroyModule")
		);
		if (destroy) destroy(it->second.instance);

		IO::DynamicLibrary::UnLoad(it->second.handle);
		m_modules.erase(it);
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