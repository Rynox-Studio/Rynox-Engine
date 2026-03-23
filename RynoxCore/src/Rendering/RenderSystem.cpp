#include <Rendering/RenderSystem.h>

namespace Rynox
{
	void RenderSystem::SetRenderer(IRenderer* r)
	{
		m_renderer = r;
	}

	IRenderer* RenderSystem::GetRenderer() const
	{
		return m_renderer;
	}

	void RenderSystem::OnAttach()
	{
	}

	void RenderSystem::OnDetach()
	{
	}

	void RenderSystem::OnUpdate(float dt)
	{
		m_renderer->BeginFrame({});
		m_renderer->EndFrame();
	}

	void RenderSystem::OnEvent(IEvent& event)
	{
	}
}