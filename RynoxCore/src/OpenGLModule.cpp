#include "Core/Modules/OpenGLModule.h"
#include "Common/Logger.h"

namespace Rynox::Core::Module {
    OpenGLModule::OpenGLModule(const std::string& modulesPath, const std::string& moduleName)
    {
		this->m_name = moduleName;
#ifdef _WIN32
		this->m_path = modulesPath + "\\" + moduleName;
#else
		this->m_path = modulesPath + "/" + moduleName;
#endif // _WIN32
    }

    bool OpenGLModule::Load()
    {
		this->m_data.libHandle = IO::DynamicLibrary::Load(this->m_path);
        if (!this->m_data.libHandle) {
            RNX_LOG_ERROR("[OpenGLModule] Failed to load OpenGL module: \"{}\"", this->m_path);
			return false;
        }

		this->m_data.createFunc = reinterpret_cast<CreateRendererFunc>(IO::DynamicLibrary::GetSymbol(this->m_data.libHandle, "CreateRenderer"));
        if (!this->m_data.createFunc) {
            RNX_LOG_ERROR("[OpenGLModule] Failed to find CreateRenderer function in OpenGL module: \"{}\"", this->m_path);
            this->UnLoad();
            return false;
        }

		this->m_data.destroyFunc = reinterpret_cast<DestroyRendererFunc>(IO::DynamicLibrary::GetSymbol(this->m_data.libHandle, "DestroyRenderer"));
        if (!this->m_data.destroyFunc) {
            RNX_LOG_ERROR("[OpenGLModule] Failed to find DestroyRenderer function in OpenGL module: \"{}\"", this->m_path);
            this->UnLoad();
            return false;
        }

		this->m_data.renderer = this->m_data.createFunc();
		if (!this->m_data.renderer) {
            RNX_LOG_ERROR("[OpenGLModule] Failed to create OpenGL renderer from module: \"{}\"", this->m_path);
            this->UnLoad();
			return false;
		}
        RNX_LOG_INFO("[OpenGLModule] OpenGL module loaded successfully: \"{}\"", this->m_path);
		return true;
    }

    void OpenGLModule::UnLoad()
    {
		if (this->m_data.destroyFunc && this->m_data.renderer) {
			this->m_data.destroyFunc(this->m_data.renderer);
			this->m_data.renderer = nullptr;
		}
		if (this->m_data.libHandle) {
			IO::DynamicLibrary::UnLoad(this->m_data.libHandle);
			this->m_data.libHandle = nullptr;
            RNX_LOG_WARNING("[OpenGLModule] OpenGL module unloaded: \"{}\"", this->m_path);
		}
    }

    IRenderer* OpenGLModule::GetRenderer() const
    {
        return this->m_data.renderer;
    }

    OpenGLModule::~OpenGLModule()
    {
		this->UnLoad();
    }
}
