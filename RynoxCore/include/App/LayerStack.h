#pragma once

#include <cstdint>
#include <vector>
#include <memory>

#include "ILayer.h"

namespace Rynox
{
	class LayerStack
	{
	public:
		LayerStack() = default;
		~LayerStack() = default;

		ILayer* PushLayer(std::unique_ptr<ILayer> layer);
		void RemoveLayer(ILayer* layer);

		auto begin() { return m_Layers.begin(); }
		auto begin() const { return m_Layers.begin(); }
		auto cbegin() const { return m_Layers.begin(); }

		auto end() { return m_Layers.end(); }
		auto end() const { return m_Layers.end(); }
		auto cend() const { return m_Layers.end(); }

	private:
		std::vector<std::unique_ptr<ILayer>> m_Layers;
	};
}
