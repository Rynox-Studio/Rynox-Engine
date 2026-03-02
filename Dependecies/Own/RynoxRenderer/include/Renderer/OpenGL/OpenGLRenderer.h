#pragma once

#include <memory>

#include "Renderer/IRenderer.h"
#include "Renderer/OpenGL/IGraphicsContext.h"
#include "Renderer/OpenGL/OpenGLDevice.h"
#include "Renderer/OpenGL/ResourceService.h"

namespace Rynox::Renderer::OpenGL
{
	class OpenGLRenderer : public IRenderer
	{
	public:
		~OpenGLRenderer();
		bool Initialize(void* nWindow, void* nDisplay) override;

		void BeginFrame() override;
		void RenderFrame(const Graphics::FrameContext& ctx) override;
		void EndFrame() override;

		void SetViewport(int x, int y, int width, int height) override;
		void SetClearColor(float r, float g, float b, float a) override;
		MeshHandle LoadMesh(const Graphics::MeshData mesh) override;
	private:
		std::unique_ptr<IGraphicsContext> m_context;
		std::unique_ptr<OpenGLDevice> m_device;
		std::unique_ptr<ResourceService> m_resource;
		bool m_isInitialized = false;
	};
}