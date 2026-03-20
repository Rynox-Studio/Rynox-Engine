#pragma once

#include <Common/Macros.h>

#include "Core/Graphics/IRenderer.h"
#include "Renderer/OpenGL/IGraphicsContext.h"
#include "Renderer/OpenGL/OpenGLResourceService.h"

namespace Rynox::Renderer::OpenGL
{
	struct RenderCommand
	{
		RenderCommand(Graphics::MeshHandle mesh, Graphics::ShaderHandle shader, Math::Mat4 model) : mesh(mesh), shader(shader), model(model) {}
		Graphics::MeshHandle mesh;
		Graphics::ShaderHandle shader;
		Math::Mat4 model;
	};

	class RNX_RENDERER_API OpenGLRenderer final : public IRenderer
	{
	public:
		OpenGLRenderer();
		~OpenGLRenderer();

		bool Initialize(const RendererDesc& desc) override;
		const RendererDesc& GetDesc() const;

		void BeginFrame(const Graphics::FrameData& frame) override;
		void Submit(const Graphics::DrawCommand& cmd);
		void EndFrame() override;

		bool SetOutputSize(uint32_t width, uint32_t height) override;
		void SetViewport(const Viewport& viewport) override;
		void SetClearColor(const Math::Vec4& color) override;

		Graphics::MeshHandle LoadMesh(const Graphics::MeshData& mesh) override;
		Graphics::ShaderHandle LoadShader(const Graphics::ShaderData& shader) override;
	private:
		std::unique_ptr<IGraphicsContext> m_pContext;
		std::unique_ptr<OpenGLResourceService> m_pResourceService;
		std::vector<Graphics::DrawCommand> m_drawCommandList;

		RendererDesc m_desc;
		Graphics::FrameData m_frameData;

		bool m_initialized = false;
	};
}