#pragma once

#include "Core/Graphics/IRenderer.h"
#include "Core/Interfaces/ISystem.h"

namespace Rynox::Core::System
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