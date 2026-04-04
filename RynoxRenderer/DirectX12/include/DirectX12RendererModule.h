#pragma once

#include <Rendering/IRendererModule.h>
#include <Rendering/IRenderer.h>

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

        const char* GetName() const override { return "Renderer-DX12"; }
        const char* GetVersion() const override { return "1.0.0"; }

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