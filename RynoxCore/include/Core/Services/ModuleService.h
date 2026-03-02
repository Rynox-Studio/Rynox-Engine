#pragma once

#include <type_traits>
#include <functional>

enum class ModuleType {
	OpenGLRenderer = 0
};


namespace std {
	template <>
	struct hash<ModuleType> {
		size_t operator()(const ModuleType& key) const noexcept {
			return std::hash<std::underlying_type_t<ModuleType>>{}(
				static_cast<std::underlying_type_t<ModuleType>>(key));
		}
	};
}

#include <memory>
#include <unordered_map>
#include "Core/Interfaces/IModule.h"
#include "Core/Interfaces/IService.h"

namespace Rynox::Core::Service {
	class ModuleService : public IService {
	public:
		enum class ErrorCode {
			None,
			FailedToLoadOpenGL,
			ModuleAlreadyLoaded,
			UnknownModuleType
		};

		bool Initialize() noexcept(true) override;

		ModuleService(std::string modulesPath);
		~ModuleService();

		ErrorCode LoadModule(ModuleType type);
		void UnLoadModule(ModuleType type);
		IModule* GetModule(ModuleType type);
	private:
		ModuleService(const ModuleService&) = delete;
		ModuleService& operator=(const ModuleService&) = delete;
		ModuleService(ModuleService&&) = delete;
		ModuleService& operator=(ModuleService&&) = delete;

		std::unordered_map<ModuleType, std::unique_ptr<IModule>> m_modules;
		std::string m_modulesPath;
	};
}
