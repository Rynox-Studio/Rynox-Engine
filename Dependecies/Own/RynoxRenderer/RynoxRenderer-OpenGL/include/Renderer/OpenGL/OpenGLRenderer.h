#pragma once

#include "Core/Interfaces/IRenderer.h"
#include "Renderer/OpenGL/IGraphicsContext.h"
#include "Renderer/OpenGL/OpenGLDevice.h"
#include "Renderer/OpenGL/ResourceService.h"

namespace Rynox::Renderer::OpenGL
{
	class RNX_RENDERER_API OpenGLRenderer : public IRenderer
	{
	public:
		OpenGLRenderer();
		~OpenGLRenderer();
		bool Initialize(void* nWindow, void* nDisplay) override;

		void BeginFrame() override;
		void RenderFrame(const Graphics::FrameContext& ctx) override;
		void EndFrame() override;

		void SetViewport(int x, int y, int width, int height) override;
		void SetClearColor(float r, float g, float b, float a) override;
		MeshHandle LoadMesh(const Graphics::MeshData mesh) override;
	private:
		struct Impl;
		Impl* m_impl;
	};
}

#ifdef __cplusplus
extern "C" {
#endif

	RNX_RENDERER_API Rynox::IRenderer* CreateRenderer();
	RNX_RENDERER_API void DestroyRenderer(Rynox::IRenderer* renderer);

#ifdef __cplusplus
}
#endif