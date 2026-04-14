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
	void OpenGLRenderer::BeginFrame(const FrameData& frame)
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		m_frameData = frame;
		m_pResourceService->ProcessQueues();
	}
	void OpenGLRenderer::Submit(const DrawCommand& cmd)
	{
		m_drawCommandList.push_back(cmd);
	}
	void OpenGLRenderer::EndFrame()
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		Math::Mat4 VP = m_frameData.camera.projMatrixLH * m_frameData.camera.viewMatrixLH;
		for (const auto& cmd : m_drawCommandList)
		{
			auto pShader = m_pResourceService->GetResource<OpenGLShader>(cmd.Shader);
			auto pMesh = m_pResourceService->GetResource<OpenGLMesh>(cmd.Geometry);

			if (pShader && pMesh)
			{
				OpenGLDevice::BindShader(*pShader);
				OpenGLDevice::BindVertexArray(pMesh->vao);

				Math::Mat4 MVP = VP * cmd.Transform;
				OpenGLDevice::UniformMatrix4fv(*pShader, "uMVP", MVP.data);

				OpenGLDevice::DrawElementsBaseVertex(cmd.SubMesh.indexCount, cmd.SubMesh.indexOffset, cmd.SubMesh.baseVertex);

				OpenGLDevice::UnBindVertexArray();
				OpenGLDevice::UnBindShader();
			}
			else
			{
				RNX_LOG_ERROR("[OpenGL] Failed to retrieve resources for DrawCommand.");
			}
		}
		m_drawCommandList.clear();
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
	GeometryHandle OpenGLRenderer::LoadMesh(const MeshData& mesh)
	{
		return m_pResourceService->QueueMesh(mesh);
	}
	ShaderHandle OpenGLRenderer::LoadShader(const ShaderData& shader)
	{
		return m_pResourceService->QueueShader(shader);
	}
}
