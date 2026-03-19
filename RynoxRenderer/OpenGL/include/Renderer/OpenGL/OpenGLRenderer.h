#pragma once

#include <Common/Macros.h>

#include "Core/Graphics/IRenderer.h"
#include "Renderer/OpenGL/IGraphicsContext.h"
#include "Renderer/OpenGL/OpenGLResourceService.h"

namespace Rynox::Renderer::OpenGL
{
	class RNX_RENDERER_API OpenGLRenderer final : public IRenderer
	{
	public:
		OpenGLRenderer();
		~OpenGLRenderer();

		bool Initialize(const RendererDesc& desc) override;
		const RendererDesc& GetDesc() const;

		void BeginFrame() override;
		void EndFrame() override;

		bool SetOutputSize(uint32_t width, uint32_t height) override;
		void SetViewport(const Viewport& viewport) override;
		void SetClearColor(const Math::Vec4& color) override;

		void DrawMesh(Graphics::MeshHandle mesh, Graphics::ShaderHandle shader) override;

		Graphics::MeshHandle LoadMesh(const Graphics::MeshData& mesh) override;
		Graphics::ShaderHandle LoadShader(const Graphics::ShaderData& shader) override;
	private:
		struct Impl;
		Impl* m_impl;
	};
}