#include "Renderer/OpenGL/OpenGLRenderer.h"

#include <glad/glad.h>
#include <Renderer/Platform/Win32GLContext.h>
#include <Common/Logger.h>

namespace Rynox::Renderer::OpenGL
{
	void OpenGLRenderer::Initialize(void* nWindow, void* nDisplay)
	{
		m_pContext = std::make_unique<Platform::Win32GLContext>((static_cast<HWND>(nWindow)));
		m_pContext->Init();

		if (!gladLoadGLLoader((GLADloadproc)m_pContext->GetOpenGLLoader())) {
			RNX_LOG_ERROR("error init glad");
		}
	}
	void OpenGLRenderer::BeginFrame()
	{
	}
	void OpenGLRenderer::RenderFrame(float time)
	{
	}
	void OpenGLRenderer::EndFrame()
	{
		m_pContext->SwapBuffers();
	}
	OpenGLRenderer::~OpenGLRenderer()
	{
	}
}