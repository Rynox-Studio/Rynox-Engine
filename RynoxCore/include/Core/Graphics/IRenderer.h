#pragma once

#include <Common/Macros.h>

#include <Core/Graphics/GraphicsData.h>
#include <Core/Graphics/GPUHandle.h>
#include <Core/Graphics/FrameData.h>
#include <Core/Graphics/DrawCommand.h>

#include <Math/Vec4.h>

namespace Rynox
{
	enum class GraphicsAPI
	{
		OpenGL,
		DirectX12,

#ifdef RNX_WINDOWS
		Default = DirectX12,
#else
		Default = OpenGL,
#endif
		Count
	};

	struct Viewport
	{
		uint32_t x = 0;
		uint32_t y = 0;
		uint32_t width = 0;
		uint32_t height = 0;
	};

	struct RendererDesc
	{
		void* nWindow = nullptr;
		void* nDisplay = nullptr;
		Viewport viewport = { 0, 0, 0, 0 };
		uint32_t outputWidth = 0;
		uint32_t outputHeight = 0;
	};

	class IRenderer
	{
	public:
		virtual ~IRenderer() noexcept(false) = default;

		virtual bool Initialize(const RendererDesc& desc) = 0;
		virtual const RendererDesc& GetDesc() const = 0;

		virtual void BeginFrame(const Graphics::FrameData& frame) = 0;
		virtual void Submit(const Graphics::DrawCommand& cmd) = 0;
		virtual void EndFrame() = 0;

		virtual bool SetOutputSize(uint32_t width, uint32_t height) = 0;
		virtual void SetViewport(const Viewport& viewport) = 0;
		virtual void SetClearColor(const Math::Vec4& color) = 0;

		virtual Graphics::MeshHandle LoadMesh(const Graphics::MeshData& mesh) = 0;
		virtual Graphics::ShaderHandle LoadShader(const Graphics::ShaderData& shader) = 0;
	};
}