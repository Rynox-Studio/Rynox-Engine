#pragma once

#include <memory>

#include "Renderer/IRenderer.h"
#include "Renderer/OpenGL/IGraphicsContext.h"

namespace Rynox::Renderer::OpenGL
{
	class OpenGLRenderer : public IRenderer
	{
	public:
		void Initialize(void* nWindow, void* nDisplay) override;
		void BeginFrame() override;
		void RenderFrame(float time) override;
		void EndFrame() override;
		~OpenGLRenderer();
	private:
		std::unique_ptr<IGraphicsContext> m_pContext;
	};
}