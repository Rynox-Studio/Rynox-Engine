#pragma once

#include <unordered_map>
#include <string>

#include "Core/IO/DynamicLibrary.h"
#include "Core/Interfaces/IModule.h"
#include "Core/Interfaces/IService.h"

namespace Rynox::Core::Service
{
    class ModuleService : public IService
    {
    public:
        ~ModuleService();

        bool Initialize() noexcept(true);

        bool LoadModule(const std::string& path, const std::string& name);
        bool UnloadModule(const std::string& name);

        IModule* GetModule(const std::string& name);
    private:
        struct ModuleInfo {
            LibHandle handle;
            IModule* instance;
        };

        std::unordered_map<std::string, ModuleInfo> m_modules;
    };
}