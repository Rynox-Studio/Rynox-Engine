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
	bool RenderSystem::Initialize() noexcept(true)
	{
		if (!m_renderer)
			return false;

		return true;
	}
	void RenderSystem::OnUpdate(float dt, float time)
	{
		if (!m_renderer)
			return;

		m_renderer->BeginFrame({});
		m_renderer->EndFrame();
	}
}