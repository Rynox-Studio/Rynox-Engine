#include "DirectX12RendererModule.h"

namespace Rynox
{
	DirectX12RendererModule::DirectX12RendererModule()
	{
		m_renderer = new Renderer::Dx12Renderer();
	}
	DirectX12RendererModule::~DirectX12RendererModule()
	{
		delete m_renderer;
	}
	bool DirectX12RendererModule::Initialize()
	{
		return m_renderer != nullptr;
	}
	void DirectX12RendererModule::Shutdown()
	{
	}
	std::string DirectX12RendererModule::GetName() const
	{
		return std::string("Dx12Renderer");
	}
	std::string DirectX12RendererModule::GetVersion() const
	{
		return std::string("1.0.0");
	}
	Rynox::IRenderer* DirectX12RendererModule::GetRenderer()
	{
		return m_renderer;
	}
	Rynox::IRendererModule* CreateModule()
	{
		return new DirectX12RendererModule();
	}
	void DestroyModule(Rynox::IRendererModule* module)
	{
		delete module;
	}
}