#include "Renderer/OpenGL/OpenGLRenderer.h"

#include <memory>
#include <glad/glad.h>
#include <Common/Logger.h>

#include "Renderer/Platform/Win32GLContext.h"
#include "Renderer/OpenGL/OpenGLDevice.h"

namespace Rynox::Renderer::OpenGL
{
    struct OpenGLRenderer::Impl
    {
        std::unique_ptr<IGraphicsContext> context;
        std::unique_ptr<OpenGLResourceService> resource;
		RendererDesc desc;
        bool isInitialized = false;
    };
    OpenGLRenderer::OpenGLRenderer() : m_impl(new Impl)
    {
    }
    OpenGLRenderer::~OpenGLRenderer()
    {
        delete m_impl;
    }
	bool OpenGLRenderer::Initialize(const RendererDesc& desc)
	{
		if (!m_impl->isInitialized)
		{
			m_impl->context = std::make_unique<Platform::Win32GLContext>((static_cast<HWND>(desc.nWindow)));
			if (!m_impl->context->Init())
			{
				RNX_LOG_ERROR("[OpenGL] Failed to initialize Context.");
				return false;
			}

			if (!gladLoadGLLoader((GLADloadproc)m_impl->context->GetOpenGLLoader())) {
				RNX_LOG_ERROR("[OpenGL] Failed to initialize GLAD.");
				return false;
			}

			m_impl->resource = std::make_unique<OpenGLResourceService>();

			SetClearColor(Math::Vec4(0.0f, 0.0f, 0.0f, 1.0f));
			glEnable(GL_DEPTH_TEST);
			m_impl->isInitialized = true;
		}

		return true;
	}
	const RendererDesc& OpenGLRenderer::GetDesc() const
	{
		return m_impl->desc;
	}
	void OpenGLRenderer::BeginFrame()
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		m_impl->resource->ProcessQueues();
	}
	void OpenGLRenderer::EndFrame()
	{
        m_impl->context->SwapBuffers();
	}
	bool OpenGLRenderer::SetOutputSize(uint32_t width, uint32_t height)
	{
		return true;
	}
	void OpenGLRenderer::SetViewport(const Viewport& viewport)
	{
		glViewport(viewport.x, viewport.y, viewport.width, viewport.height);
		m_impl->desc.viewport = viewport;
	}
	void OpenGLRenderer::SetClearColor(const Math::Vec4& color)
	{
		glClearColor(color.r, color.g, color.b, color.a);
	}
	void OpenGLRenderer::DrawMesh(Graphics::MeshHandle mesh, Graphics::ShaderHandle shader)
	{
	}
	Graphics::MeshHandle OpenGLRenderer::LoadMesh(const Graphics::MeshData& mesh)
	{
		return Graphics::MeshHandle();
	}
	Graphics::ShaderHandle OpenGLRenderer::LoadShader(const Graphics::ShaderData& shader)
	{
		return Graphics::ShaderHandle();
	}
}
