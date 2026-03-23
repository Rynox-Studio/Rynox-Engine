#pragma once

#include <unordered_map>
#include <string>

#include "Base/IModule.h"
#include "Base/IService.h"

namespace Rynox
{
    class ModuleService : public IService
    {
    public:
        ~ModuleService() override { Shutdown(); }

        bool Initialize() override;
        void Shutdown() override;

        bool LoadModule(const std::string& path, const std::string& name);
        bool UnloadModule(const std::string& name);

        IModule* GetModule(const std::string& name);
    private:
        struct ModuleInfo 
        {
            void* Library;
            IModule* Instance;
        };

        std::unordered_map<std::string, ModuleInfo> m_modules;
    };
}