#include "pch.h"
#include "layer_stack.h"

namespace Donut {

	LayerStack::LayerStack()
	{
		//layer_insert_ = layers_.begin();
	}

	LayerStack::~LayerStack()
	{
		for (Layer* layer : layers_)
			delete layer;
	}

	void LayerStack::pushLayer(Layer* layer)
	{
		layers_.emplace(layers_.begin() + layer_insert_idx_, layer);
		layer_insert_idx_++;
		//layer->onAttach();
	}

	void LayerStack::pushOverlay(Layer* overlay)
	{
		layers_.emplace_back(overlay);
		//overlay->onAttach();
	}

	void LayerStack::popLayer(Layer* layer)
	{
		auto it = std::find(layers_.begin(), layers_.end(), layer);
		if (it != layers_.end())
		{
			layers_.erase(it);
			layer_insert_idx_--;
		}
	}

	void LayerStack::popOverlay(Layer* overlay)
	{
		auto it = std::find(layers_.begin(), layers_.end(), overlay);
		if (it != layers_.end())
			layers_.erase(it);
	}

}