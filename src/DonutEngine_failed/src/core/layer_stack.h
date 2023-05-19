#ifndef LAYER_STACK_H
#define LAYER_STACK_H

#include "core/core.h"
#include "layer.h"

namespace Donut
{
	class DONUT_API LayerStack
	{
	public:
		LayerStack();
		~LayerStack();

		void pushLayer(Layer* layer);
		void pushOverlay(Layer* overlay);
		void popLayer(Layer* layer);
		void popOverlay(Layer* overlay);

		std::vector<Layer*>::iterator begin() { return layers_.begin(); }
		std::vector<Layer*>::iterator end() { return layers_.end(); }
	private:
		std::vector<Layer*> layers_;
		std::vector<Layer*>::iterator layer_insert_;
		int layer_insert_idx_ = 0;
	};
}


#endif