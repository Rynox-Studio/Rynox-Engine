#pragma once

#include <Core/Interfaces/IRendererModule.h>
#include <Core/Graphics/IRenderer.h>

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
        std::string GetName() const override;
        std::string GetVersion() const override;
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