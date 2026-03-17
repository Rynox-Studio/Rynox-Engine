#pragma once

#include "Core/Graphics/GraphicsData.h"
#include "Core/Graphics/GPUHandle.h"

#include <Math/Vec4.h>

namespace Rynox
{
	struct Viewport
	{
		int x;
		int y;
		int width;
		int height;
	};

	struct RendererDesc
	{
		void* nWindow;
		void* nDisplay;
		Viewport viewport;
	};

	class IRenderer
	{
	public:
		virtual ~IRenderer() = default;
		virtual bool Initialize(RendererDesc desc) = 0;

		virtual void BeginFrame() = 0;
		virtual void EndFrame() = 0;

		virtual void SetOutputSize(uint32_t width, uint32_t height) = 0;
		virtual void SetViewport(Viewport viewport) = 0;
		virtual void SetClearColor(Math::Vec4 color) = 0;

		virtual void DrawMesh(Graphics::MeshHandle mesh, Graphics::ShaderHandle shader) = 0;

		virtual Graphics::MeshHandle LoadMesh(const Graphics::MeshData& mesh) = 0;
		virtual Graphics::ShaderHandle LoadShader(const Graphics::ShaderData& shader) = 0;
	};

	using CreateRendererFn = IRenderer* (*)();
	using DestroyRendererFn = void (*)(IRenderer*);
}