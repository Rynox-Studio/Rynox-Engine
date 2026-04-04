#pragma once

#include <Common/Macros.h>

#include "GraphicsData.h"
#include "GPUHandle.h"
#include "FrameData.h"
#include "DrawCommand.h"

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

		virtual bool Initialize(const RendererDesc& desc) noexcept(false) = 0;
		virtual const RendererDesc& GetDesc() const = 0;

		virtual void BeginFrame(const FrameData& frame) = 0;
		virtual void Submit(const DrawCommand& cmd) = 0;
		virtual void EndFrame() = 0;

		virtual bool SetOutputSize(uint32_t width, uint32_t height) = 0;
		virtual void SetViewport(const Viewport& viewport) = 0;
		virtual void SetClearColor(const Math::Vec4& color) = 0;

		virtual GeometryHandle LoadMesh(const MeshData& mesh) = 0;
		virtual ShaderHandle LoadShader(const ShaderData& shader) = 0;
	};
}