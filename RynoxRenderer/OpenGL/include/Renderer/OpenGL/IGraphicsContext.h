#pragma once

namespace Rynox::Renderer::OpenGL
{
	class IGraphicsContext
	{
	public:
		virtual ~IGraphicsContext() = default;
		virtual bool Init() = 0;
		virtual void* GetOpenGLLoader() = 0;
		virtual void SwapBuffers() = 0;
		virtual bool MakeCurrent() = 0;
	};
}