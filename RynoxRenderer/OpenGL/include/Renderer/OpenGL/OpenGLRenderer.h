#pragma once

#include <Common/Macros.h>

#include "Core/Graphics/IRenderer.h"
#include "Renderer/OpenGL/IGraphicsContext.h"
#include "Renderer/OpenGL/OpenGLResourceService.h"

namespace Rynox::Renderer::OpenGL
{
	class RNX_RENDERER_API OpenGLRenderer : public IRenderer
	{
	public:
		OpenGLRenderer();
		~OpenGLRenderer();

		bool Initialize(RendererDesc desc) override;
		const RendererDesc& GetDesc() const;

		void BeginFrame() override;
		void EndFrame() override;

		bool SetOutputSize(uint32_t width, uint32_t height);
		void SetViewport(Viewport viewport);
		void SetClearColor(Math::Vec4 color);

		void DrawMesh(Graphics::MeshHandle mesh, Graphics::ShaderHandle shader);

		Graphics::MeshHandle LoadMesh(const Graphics::MeshData& mesh);
		Graphics::ShaderHandle LoadShader(const Graphics::ShaderData& shader);
	private:
		struct Impl;
		Impl* m_impl;
	};
}