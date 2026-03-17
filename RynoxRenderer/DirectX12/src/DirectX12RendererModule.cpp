#include "DirectX12RendererModule.h"

namespace Rynox
{
	OpenGLRendererModule::OpenGLRendererModule()
	{
		m_renderer = new Renderer::Dx12Renderer();
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
	std::string OpenGLRendererModule::GetName() const
	{
		return std::string("Dx12Renderer");
	}
	std::string OpenGLRendererModule::GetVersion() const
	{
		return std::string("1.0.0");
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