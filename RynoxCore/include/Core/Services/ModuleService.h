#pragma once

#include <unordered_map>
#include <string>

#include "Core/IO/DynamicLibrary.h"
#include "Core/Interfaces/IModule.h"

namespace Rynox::Core
{
    class ModuleService
    {
    public:
        ~ModuleService();

        bool LoadModule(const std::string& path);
        void UnloadModule(const std::string& name);

        IModule* GetModule(const std::string& name);

    private:
        struct ModuleInfo {
            LibHandle handle;
            IModule* instance;
        };

        std::unordered_map<std::string, ModuleInfo> m_modules;
    };
}