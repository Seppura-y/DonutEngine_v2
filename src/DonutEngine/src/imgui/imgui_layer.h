#ifndef IMGUI_LAYER_H
#define IMGUI_LAYER_H
#include "core/layer.h"

namespace Donut
{
	class DONUT_API ImGuiLayer : public Layer
	{
	public:
		ImGuiLayer();
		~ImGuiLayer();

		void onAttach() override;
		void onDetach() override;
		void onUpdate() override;
		void onEvent(Event& ev) override;
	private:
		float time_ = 0.0f;
	};
}

#endif