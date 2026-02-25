#pragma once

#include <vector>
#include <cstdint>

#include "ILayer.h"

namespace Rynox::Core 
{
	class LayerStack 
	{
	public:
		using size_type = uint32_t;

		using iterator = std::vector<ILayer*>::iterator;
		using reverse_iterator = std::vector<ILayer*>::reverse_iterator;
		using const_iterator = std::vector<ILayer*>::const_iterator;
		using const_reverse_iterator = std::vector<ILayer*>::const_reverse_iterator;

	public:
		LayerStack();
		~LayerStack();

		void PushLayer(ILayer* layer);
		void PushOverlay(ILayer* overlay);

		void PopLayer(ILayer* layer);
		void PopOverlay(ILayer* overlay);

		iterator begin();
		iterator end();
		const_iterator begin() const;
		const_iterator end() const;

		reverse_iterator rbegin();
		reverse_iterator rend();
		const_reverse_iterator rbegin() const;
		const_reverse_iterator rend() const;

	private:
		std::vector<ILayer*> m_Layers;
		uint32_t m_LayerInsertIndex = 0;
	};
}
