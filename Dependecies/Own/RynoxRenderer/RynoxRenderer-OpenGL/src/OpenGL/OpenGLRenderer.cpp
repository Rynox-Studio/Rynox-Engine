#include "Renderer/OpenGL/OpenGLRenderer.h"

#include <glad/glad.h>
#include <Renderer/Platform/Win32GLContext.h>
#include <Common/Logger.h>

#include "Core/Graphics/MeshData.h"
#include "Renderer/OpenGL/OpenGLMesh.h"

namespace Rynox::Renderer::OpenGL
{
    struct OpenGLRenderer::Impl
    {
        std::unique_ptr<IGraphicsContext> context;
        std::unique_ptr<OpenGLDevice> device;
        std::unique_ptr<ResourceService> resource;
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

            m_impl->device = std::make_unique<OpenGLDevice>();
            m_impl->resource = std::make_unique<ResourceService>();

			SetClearColor(0.1, 0.1, 0.1, 1.0);
            m_impl->isInitialized = true;
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

            shader = m_impl->device->CreateShader(vertexSrc, fragmentSrc);

            initialized = true;
        }

        const OpenGLMesh& mesh = m_impl->resource->GetMesh(triangleMesh);

        m_impl->device->BindShader(shader);
        m_impl->device->BindVertexArray(mesh.vao);

        glDrawElements(
            GL_TRIANGLES,
            mesh.indexCount,
            GL_UNSIGNED_INT,
            nullptr
        );

        m_impl->device->UnBindVertexArray();
        m_impl->device->UnBindShader();
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
	MeshHandle OpenGLRenderer::LoadMesh(const Graphics::MeshData mesh)
	{
		VertexBufferHandle vbo = m_impl->device->CreateVertexBuffer(mesh.vertices, mesh.vertexSize, mesh.layout);
		IndexBufferHandle ibo = m_impl->device->CreateIndexBuffer(mesh.indices, mesh.indexCount);
		VertexArrayHandle vao = m_impl->device->CreateVertexArray(vbo, ibo);

		OpenGLMesh glMesh(vbo, ibo, vao, mesh.indexCount);
		return m_impl->resource->AddMesh(glMesh);
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
