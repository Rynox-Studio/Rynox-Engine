#pragma once

#include <Rendering/IRendererModule.h>
#include <Rendering/IRenderer.h>

#include <Common/Macros.h>

#include <Renderer/OpenGL/OpenGLRenderer.h>

namespace Rynox
{
    class OpenGLRendererModule : public IRendererModule
    {
    public:
        OpenGLRendererModule();
		~OpenGLRendererModule();

        bool Initialize() override;
        void Shutdown() override;
        const char* GetName() const override;
        const char* GetVersion() const override;
        Rynox::IRenderer* GetRenderer() override;
    private:
        Renderer::OpenGL::OpenGLRenderer* m_renderer;
    };

    extern "C"
    {
        RNX_RENDERER_API Rynox::IRendererModule* CreateModule();
        RNX_RENDERER_API void DestroyModule(Rynox::IRendererModule* module);
    }
}