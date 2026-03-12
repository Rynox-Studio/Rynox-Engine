#pragma once

#include "Core/Interfaces/IRenderer.h"
#include "Renderer/OpenGL/IGraphicsContext.h"
#include "Renderer/OpenGL/OpenGLResourceService.h"

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

		Graphics::MeshHandle QueueMesh(const Graphics::MeshData& mesh);
		Graphics::ShaderHandle QueueShader(const Graphics::ShaderData& shader);
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