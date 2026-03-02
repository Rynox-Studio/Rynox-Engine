#pragma once

#include "Core/Graphics/FrameContext.h"
#include "Core/Graphics/MeshData.h"
#include "Core/Graphics/RenderTypes.h"

namespace Rynox
{
	class IRenderer
	{
	public:
		virtual ~IRenderer() = default;
		virtual bool Initialize(void* nWindow, void* nDisplay) = 0;

		virtual void BeginFrame() = 0;
		virtual void RenderFrame(const Graphics::FrameContext& ctx) = 0;
		virtual void EndFrame() = 0;

		virtual void SetViewport(int x, int y, int width, int height) = 0;
		virtual void SetClearColor(float r, float g, float b, float a) = 0;
		virtual Graphics::MeshHandle LoadMesh(const Graphics::MeshData mesh) = 0;
	};

	using CreateRendererFunc = IRenderer * (*)();
	using DestroyRendererFunc = void (*)(IRenderer*);
}