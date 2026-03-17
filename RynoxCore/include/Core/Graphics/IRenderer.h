#pragma once

#include "Core/Graphics/GraphicsData.h"
#include "Core/Graphics/GPUHandle.h"

#include <Math/Vec4.h>

namespace Rynox
{
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

		virtual bool Initialize(RendererDesc desc) = 0;
		virtual const RendererDesc& GetDesc() const = 0;

		virtual void BeginFrame() = 0;
		virtual void EndFrame() = 0;

		virtual bool SetOutputSize(uint32_t width, uint32_t height) = 0;
		virtual void SetViewport(Viewport viewport) = 0;
		virtual void SetClearColor(Math::Vec4 color) = 0;

		virtual void DrawMesh(Graphics::MeshHandle mesh, Graphics::ShaderHandle shader) = 0;

		virtual Graphics::MeshHandle LoadMesh(const Graphics::MeshData& mesh) = 0;
		virtual Graphics::ShaderHandle LoadShader(const Graphics::ShaderData& shader) = 0;
	};

	using CreateRendererFn = IRenderer* (*)();
	using DestroyRendererFn = void (*)(IRenderer*);
}