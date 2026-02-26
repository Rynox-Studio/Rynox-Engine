#pragma once

namespace Rynox::Renderer
{
	class IRenderer
	{
	public:
		virtual ~IRenderer() = default;
		virtual void Initialize(void* nWindow, void* nDisplay) = 0;
		virtual void BeginFrame() = 0;
		virtual void RenderFrame(float time) = 0;
		virtual void EndFrame() = 0;
	};
}