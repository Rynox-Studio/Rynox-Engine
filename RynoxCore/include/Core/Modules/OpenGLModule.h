#pragma once

#include "Core/Interfaces/IModule.h"
#include "Core/Interfaces/IRenderer.h"
#include "Core/IO/DynamicLibrary.h"

namespace Rynox::Core::Module {
	class OpenGLModule : public IModule {
	public:
		OpenGLModule(const std::string& modulesPath, const std::string& moduleName);
		~OpenGLModule();

		virtual bool Load() override;
		virtual void UnLoad() override;
		IRenderer* GetRenderer() const;
	private:
		struct ModuleData {
			LibHandle libHandle = nullptr;
			CreateRendererFunc createFunc = nullptr;
			DestroyRendererFunc destroyFunc = nullptr;
			IRenderer* renderer = nullptr;
		};

		ModuleData m_data;
		std::string m_path;
		std::string m_name;

		OpenGLModule(const OpenGLModule&) = delete;
		OpenGLModule& operator=(const OpenGLModule&) = delete;
	};
}
