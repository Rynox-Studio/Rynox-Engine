#include "Renderer/OpenGL/OpenGLRenderer.h"

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
        bool isInitialized = false;
    };
    OpenGLRenderer::OpenGLRenderer() : m_impl(new Impl)
    {
    }
    OpenGLRenderer::~OpenGLRenderer()
    {
        delete m_impl;
    }
	bool OpenGLRenderer::Initialize(void* nWindow, void* nDisplay)
	{
		if (!m_impl->isInitialized)
		{
            m_impl->context = std::make_unique<Platform::Win32GLContext>((static_cast<HWND>(nWindow)));
			if (!m_impl->context->Init())
			{
				RNX_LOG_ERROR("error init context");
				return false;
			}

			if (!gladLoadGLLoader((GLADloadproc)m_impl->context->GetOpenGLLoader())) {
				RNX_LOG_ERROR("error init glad");
				return false;
			}

            m_impl->resource = std::make_unique<OpenGLResourceService>();

			SetClearColor(0.1, 0.1, 0.1, 1.0);
			glEnable(GL_DEPTH_TEST);
            m_impl->isInitialized = true;
		}

		return true;
	}
	void OpenGLRenderer::BeginFrame()
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		m_impl->resource->ProcessQueues();
	}
    void OpenGLRenderer::RenderFrame(const Graphics::FrameContext& ctx)
    {
		Math::Mat4 VP = ctx.camera.projMatrix * ctx.camera.viewMatrix;
		for (auto& cmd : ctx.commands)
		{
			Math::Mat4 MVP = VP * cmd.worldMatrix;

			OpenGLShader* shader = m_impl->resource->GetResource<OpenGLShader>(cmd.shader);
			if (shader != nullptr)
			{
				OpenGLDevice::BindShader(*shader);
				OpenGLDevice::UniformMatrix4fv(*shader, "uMVP", &MVP.data[0]);

				for (auto& item : cmd.items)
				{
					OpenGLMesh* mesh = m_impl->resource->GetResource<OpenGLMesh>(item.mesh);
					if (mesh != nullptr)
					{
						OpenGLDevice::BindVertexArray(mesh->vao);
						OpenGLDevice::DrawElements(mesh->count, 0);
					}
				}
			}
		}
		OpenGLDevice::UnBindVertexArray();
		OpenGLDevice::UnBindShader();
    }
	void OpenGLRenderer::EndFrame()
	{
        m_impl->context->SwapBuffers();
	}
	void OpenGLRenderer::SetViewport(int x, int y, int width, int height)
	{
		glViewport(x, y, width, height);
	}
	void OpenGLRenderer::SetClearColor(float r, float g, float b, float a)
	{
		glClearColor(r, g, b, a);
	}
	Graphics::MeshHandle OpenGLRenderer::QueueMesh(const Graphics::MeshData& mesh)
	{
		return m_impl->resource->QueueMesh(mesh);
	}
	Graphics::ShaderHandle OpenGLRenderer::QueueShader(const Graphics::ShaderData& shader)
	{
		return m_impl->resource->QueueShader(shader);
	}
}

RNX_RENDERER_API Rynox::IRenderer* CreateRenderer()
{
    return new Rynox::Renderer::OpenGL::OpenGLRenderer();
}

RNX_RENDERER_API void DestroyRenderer(Rynox::IRenderer* renderer)
{
    delete renderer;
}
