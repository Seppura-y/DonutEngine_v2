#ifndef LAYER_H
#define LAYER_H

#include "core/core.h"
#include "events/event.h"

namespace Donut
{
	class DONUT_API Layer
	{
	public:
		Layer(const std::string& name = "Layer");
		virtual ~Layer();

		virtual void onAttach() {}
		virtual void onDetach() {}
		virtual void onUpdate() {}
		virtual void onEvent(Event& event) {}

		virtual void onImGuiRender() {}

		inline const std::string& getName() const { return debug_name_; }

	protected:
		// for debug
		std::string debug_name_;
	};

}

#endif