#pragma once

#include <Common/Macros.h>

#include <Rendering/IRenderer.h>

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
		const RendererDesc& GetDesc() const override;

		void BeginFrame(const FrameData& frame) override;
		void Submit(const DrawCommand& cmd) override;
		void EndFrame() override;

		bool SetOutputSize(uint32_t width, uint32_t height) override;
		void SetViewport(const Viewport& viewport) override;
		void SetClearColor(const Math::Vec4& color) override;

		GeometryHandle LoadMesh(const MeshData& mesh) override;
		ShaderHandle LoadShader(const ShaderData& shader) override;
	private:
		std::unique_ptr<IGraphicsContext> m_pContext;
		std::unique_ptr<OpenGLResourceService> m_pResourceService;
		std::vector<DrawCommand> m_drawCommandList;

		RendererDesc m_desc;
		FrameData m_frameData;

		bool m_initialized = false;
	};
}