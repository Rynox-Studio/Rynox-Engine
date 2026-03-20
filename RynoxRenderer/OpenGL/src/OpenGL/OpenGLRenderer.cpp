#include "Renderer/OpenGL/OpenGLRenderer.h"

#include <memory>
#include <glad/glad.h>
#include <Common/Logger.h>

#include "Renderer/Platform/Win32GLContext.h"
#include "Renderer/OpenGL/OpenGLDevice.h"

namespace Rynox::Renderer::OpenGL
{
    OpenGLRenderer::OpenGLRenderer()
    {
    }
    OpenGLRenderer::~OpenGLRenderer()
    {
    }
	bool OpenGLRenderer::Initialize(const RendererDesc& desc)
	{
		if (!m_initialized)
		{
			m_pContext = std::make_unique<Platform::Win32GLContext>((static_cast<HWND>(desc.nWindow)));
			if (!m_pContext->Init())
			{
				RNX_LOG_ERROR("[OpenGL] Failed to initialize Context.");
				return false;
			}

			if (!gladLoadGLLoader((GLADloadproc)m_pContext->GetOpenGLLoader())) {
				RNX_LOG_ERROR("[OpenGL] Failed to initialize GLAD.");
				return false;
			}

			m_pResourceService = std::make_unique<OpenGLResourceService>();

			SetClearColor(Math::Vec4(0.1f, 0.1f, 0.1f, 1.0f));

			glEnable(GL_DEPTH_TEST);
			glFrontFace(GL_CCW);
			glDisable(GL_CULL_FACE);
			glClipControl(GL_LOWER_LEFT, GL_ZERO_TO_ONE);

			m_initialized = true;
		}

		return true;
	}
	const RendererDesc& OpenGLRenderer::GetDesc() const
	{
		return m_desc;
	}
	void OpenGLRenderer::BeginFrame(const Graphics::FrameData& frame)
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		m_frameData = frame;
		m_pResourceService->ProcessQueues();
	}
	void OpenGLRenderer::Submit(const Graphics::DrawCommand& cmd)
	{
		m_drawCommandList.push_back(cmd);
	}
	void OpenGLRenderer::EndFrame()
	{
        m_pContext->SwapBuffers();
	}
	bool OpenGLRenderer::SetOutputSize(uint32_t width, uint32_t height)
	{
		return true;
	}
	void OpenGLRenderer::SetViewport(const Viewport& viewport)
	{
		glViewport(viewport.x, viewport.y, viewport.width, viewport.height);
		m_desc.viewport = viewport;
	}
	void OpenGLRenderer::SetClearColor(const Math::Vec4& color)
	{
		glClearColor(color.x, color.y, color.z, color.w);
	}
	Graphics::MeshHandle OpenGLRenderer::LoadMesh(const Graphics::MeshData& mesh)
	{
		return m_pResourceService->QueueMesh(mesh);
	}
	Graphics::ShaderHandle OpenGLRenderer::LoadShader(const Graphics::ShaderData& shader)
	{
		return m_pResourceService->QueueShader(shader);
	}
}
