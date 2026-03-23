#pragma once

#include "IRenderer.h"
#include "Base/ISystem.h"

namespace Rynox
{
	class RenderSystem final : public ISystem
	{
	public:
		RenderSystem() = default;
		RenderSystem(IRenderer* r) : m_renderer(r)
		{
		}

		void SetRenderer(IRenderer* r);
		IRenderer* GetRenderer() const;

		void OnAttach() override;
		void OnDetach() override;

		void OnUpdate(float dt) override;
		void OnEvent(IEvent& event) override;

	private:
		IRenderer* m_renderer = nullptr;
	};
}