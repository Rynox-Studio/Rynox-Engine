#include "Renderer/OpenGL/OpenGLRenderer.h"

#include <glad/glad.h>
#include <Renderer/Platform/Win32GLContext.h>
#include <Common/Logger.h>

#include "Renderer/Graphics/MeshData.h"
#include "Renderer/OpenGL/OpenGLMesh.h"

namespace Rynox::Renderer::OpenGL
{
	bool OpenGLRenderer::Initialize(void* nWindow, void* nDisplay)
	{
		if (!m_isInitialized)
		{
			m_context = std::make_unique<Platform::Win32GLContext>((static_cast<HWND>(nWindow)));
			if (!m_context->Init())
			{
				RNX_LOG_ERROR("error init context");
				return false;
			}

			if (!gladLoadGLLoader((GLADloadproc)m_context->GetOpenGLLoader())) {
				RNX_LOG_ERROR("error init glad");
				return false;
			}

			m_device = std::make_unique<OpenGLDevice>();
			m_resource = std::make_unique<ResourceService>();

			SetClearColor(0.1, 0.1, 0.1, 1.0);
			m_isInitialized = true;
		}

		return true;
	}
	void OpenGLRenderer::BeginFrame()
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}
    void OpenGLRenderer::RenderFrame(const Graphics::FrameContext& ctx)
    {
        static bool initialized = false;
        static MeshHandle triangleMesh;
        static ShaderHandle shader;

        if (!initialized)
        {
            struct Vertex
            {
                float position[3];
            };

            static Vertex vertices[] =
            {
                {{ 0.0f,  0.5f, 0.0f }},
                {{-0.5f, -0.5f, 0.0f }},
                {{ 0.5f, -0.5f, 0.0f }}
            };

            static uint32_t indices[] =
            {
                0, 1, 2
            };

            static Graphics::VertexLayout layout =
            {
                {
                    { Graphics::ShaderDataType::Float3, "aPos" }
                }
            };

            Graphics::MeshData mesh{ vertices, sizeof(vertices), indices, 3, layout };

            triangleMesh = LoadMesh(mesh);

            const char* vertexSrc = R"(#version 330 core
            layout(location = 0) in vec3 aPos;

            void main()
            {
                gl_Position = vec4(aPos, 1.0);
            })";

            const char* fragmentSrc = R"(#version 330 core
            out vec4 FragColor;

            void main()
            {
                FragColor = vec4(0.2, 0.7, 1.0, 1.0);
            })";

            shader = m_device->CreateShader(vertexSrc, fragmentSrc);

            initialized = true;
        }

        const OpenGLMesh& mesh = m_resource->GetMesh(triangleMesh);

        m_device->BindShader(shader);
        m_device->BindVertexArray(mesh.vao);

        glDrawElements(
            GL_TRIANGLES,
            mesh.indexCount,
            GL_UNSIGNED_INT,
            nullptr
        );

        m_device->UnBindVertexArray();
        m_device->UnBindShader();
    }
	void OpenGLRenderer::EndFrame()
	{
		m_context->SwapBuffers();
	}
	void OpenGLRenderer::SetViewport(int x, int y, int width, int height)
	{
		glViewport(x, y, width, height);
	}
	void OpenGLRenderer::SetClearColor(float r, float g, float b, float a)
	{
		glClearColor(r, g, b, a);
	}
	MeshHandle OpenGLRenderer::LoadMesh(const Graphics::MeshData mesh)
	{
		VertexBufferHandle vbo = m_device->CreateVertexBuffer(mesh.vertices, mesh.vertexSize, mesh.layout);
		IndexBufferHandle ibo = m_device->CreateIndexBuffer(mesh.indices, mesh.indexCount);
		VertexArrayHandle vao = m_device->CreateVertexArray(vbo, ibo);

		OpenGLMesh glMesh(vbo, ibo, vao, mesh.indexCount);
		return m_resource->AddMesh(glMesh);
	}
	OpenGLRenderer::~OpenGLRenderer()
	{
	}
}