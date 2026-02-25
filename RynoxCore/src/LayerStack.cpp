#include "Core/LayerStack.h"

namespace Rynox::Core 
{
	LayerStack::LayerStack()
	{
	}

	LayerStack::~LayerStack()
	{
		for (auto& layer : m_Layers)
		{
			layer->OnDetach();
			delete layer;
		}
	}

	void LayerStack::PushLayer(ILayer* layer)
	{
		m_Layers.emplace(m_Layers.begin() + m_LayerInsertIndex, layer);
		layer->OnAttach();
		m_LayerInsertIndex++;
	}

	void LayerStack::PushOverlay(ILayer* overlay)
	{
		m_Layers.emplace_back(overlay);
		overlay->OnAttach();
	}

	void LayerStack::PopLayer(ILayer* layer)
	{
		auto it = std::find(m_Layers.begin(), m_Layers.begin() + m_LayerInsertIndex, layer);
		if (it != m_Layers.begin() + m_LayerInsertIndex)
		{
			layer->OnDetach();
			m_Layers.erase(it);
			m_LayerInsertIndex--;
		}
	}

	void LayerStack::PopOverlay(ILayer* overlay)
	{
		auto it = std::find(m_Layers.begin() + m_LayerInsertIndex, m_Layers.end(), overlay);
		if (it != m_Layers.end())
		{
			overlay->OnDetach();
			m_Layers.erase(it);
		}
	}

	LayerStack::iterator LayerStack::begin()
	{
		return m_Layers.begin();
	}

	LayerStack::iterator LayerStack::end()
	{
		return m_Layers.end();
	}

	LayerStack::const_iterator LayerStack::begin() const
	{
		return m_Layers.cbegin();
	}

	LayerStack::const_iterator LayerStack::end() const
	{
		return m_Layers.cend();
	}

	LayerStack::reverse_iterator LayerStack::rbegin()
	{
		return m_Layers.rbegin();
	}

	LayerStack::reverse_iterator LayerStack::rend()
	{
		return m_Layers.rend();
	}

	LayerStack::const_reverse_iterator LayerStack::rbegin() const
	{
		return m_Layers.crbegin();
	}

	LayerStack::const_reverse_iterator LayerStack::rend() const
	{
		return m_Layers.crend();
	}
}
