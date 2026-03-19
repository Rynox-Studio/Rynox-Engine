#pragma once

#include <Core/Interfaces/IRendererModule.h>
#include <Core/Graphics/IRenderer.h>

#include <Common/Macros.h>

#include <Dx12Renderer.h>

namespace Rynox
{
    class DirectX12RendererModule : public IRendererModule
    {
    public:
        DirectX12RendererModule();
		~DirectX12RendererModule();

        bool Initialize() override;
        void Shutdown() override;

        std::string GetName() const override;
        std::string GetVersion() const override;

        Rynox::IRenderer* GetRenderer() override;

    private:
        DirectX12::Dx12Renderer* m_renderer;
    };

    extern "C" 
    {
        RNX_RENDERER_API Rynox::IRendererModule* CreateModule();
        RNX_RENDERER_API void DestroyModule(Rynox::IRendererModule* module);
    }
}