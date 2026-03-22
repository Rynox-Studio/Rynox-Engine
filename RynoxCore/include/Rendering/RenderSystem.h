#pragma once

#include "IRenderer.h"
#include "Core/ISystem.h"

namespace Rynox
{
	class RenderSystem final : public ISystem
	{
	public:
		RenderSystem(IRenderer* r) : m_renderer(r)
		{
		}
	public:
		void SetRenderer(IRenderer* r);

		IRenderer* GetRenderer() const;
	public:
		bool Initialize() noexcept(true) override;

		void OnUpdate(float dt, float time) override;
	private:
		IRenderer* m_renderer;
	};
}