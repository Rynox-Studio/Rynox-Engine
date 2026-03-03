#include "Core/Services/ModuleService.h"
#include "Core/IO/DynamicLibrary.h"
#include <Common/Logger.h>
#include "Core/Modules/OpenGLModule.h"

namespace Rynox::Core::Service {
	bool Core::Service::ModuleService::Initialize() noexcept(true)
	{
		return true;
	}
	ModuleService::ModuleService(std::string modulesPath)
	{
		this->m_modulesPath = modulesPath;
	}
	ModuleService::ErrorCode ModuleService::LoadModule(ModuleType type)
	{
		switch (type)
		{
		case ModuleType::OpenGLRenderer:
		{
			if (m_modules.find(type) != m_modules.end()) {
				RNX_LOG_ERROR("[ModuleService] Module already loaded: OpenGLRenderer");
				return ErrorCode::ModuleAlreadyLoaded;
			}
			auto module = std::make_unique<Module::OpenGLModule>(this->m_modulesPath, RYNOX_OPENGL_MODULE_FILENAME);
			if (!module->Load()) {
				return ErrorCode::FailedToLoadOpenGL;
			}
			m_modules[type] = std::move(module);
			return ErrorCode::None;
		}
		default:
			RNX_LOG_ERROR("[ModuleService] Unknown module type");
			return ErrorCode::UnknownModuleType;
		}
	}
	void ModuleService::UnLoadModule(ModuleType type)
	{
		auto it = m_modules.find(type);
		if (it != m_modules.end()) {
			it->second->UnLoad();
			m_modules.erase(it);
			RNX_LOG_INFO("[ModuleService] Module unloaded successfully: \"{}\"", static_cast<int>(type));
		}
		else {
			RNX_LOG_ERROR("[ModuleService] Module not found: \"{}\"", static_cast<int>(type));
		}
	}
	IModule* ModuleService::GetModule(ModuleType type)
	{
		auto it = m_modules.find(type);
		if (it != m_modules.end()) {
			return it->second.get();
		}
		return nullptr;
	}
	ModuleService::~ModuleService()
	{
		for (auto& pair : m_modules) {
			pair.second->UnLoad();
		}
		m_modules.clear();
	}
}