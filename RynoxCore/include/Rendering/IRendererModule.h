#pragma once

#include "Base/IModule.h"
#include "Rendering/IRenderer.h"

namespace Rynox {
	class IRendererModule : public IModule {
	public:
		virtual Rynox::IRenderer* GetRenderer() = 0;
	};
}