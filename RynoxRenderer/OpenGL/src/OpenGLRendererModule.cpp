#include "OpenGLRendererModule.h"

namespace Rynox
{
	OpenGLRendererModule::OpenGLRendererModule()
	{
		m_renderer = new Renderer::OpenGL::OpenGLRenderer();
	}
	OpenGLRendererModule::~OpenGLRendererModule()
	{
		delete m_renderer;
	}
	bool OpenGLRendererModule::Initialize()
	{
		return m_renderer != nullptr;
	}
	void OpenGLRendererModule::Shutdown()
	{
	}
	const char* OpenGLRendererModule::GetName() const
	{
		return "OpenGLRenderer";
	}
	const char* OpenGLRendererModule::GetVersion() const
	{
		return "1.0.0";
	}
	Rynox::IRenderer* OpenGLRendererModule::GetRenderer()
	{
		return m_renderer;
	}
	Rynox::IRendererModule* CreateModule()
	{
		return new OpenGLRendererModule();
	}
	void DestroyModule(Rynox::IRendererModule* module)
	{
		delete module;
	}
}