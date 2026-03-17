#pragma once

#include "IModule.h"
#include "Core/Graphics/IRenderer.h"

namespace Rynox {
	class IRendererModule : public IModule {
	public:
		virtual Rynox::IRenderer* GetRenderer() = 0;
	};
}