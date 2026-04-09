#include <App/LayerStack.h>

#include <Common/Assert.h>

namespace Rynox
{
	ILayer* LayerStack::PushLayer(std::unique_ptr<ILayer> layer)
	{
		ILayer* ptr = layer.get();
		m_Layers.push_back(std::move(layer));
		return ptr;
	}

	void LayerStack::RemoveLayer(ILayer* layer)
	{
		for (auto it = begin(); it != end(); it++)
		{
			if (layer == it->get())
			{
				m_Layers.erase(it);
				break;
			}
		}
	}
}