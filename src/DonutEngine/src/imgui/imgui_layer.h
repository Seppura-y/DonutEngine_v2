#ifndef IMGUI_LAYER_H
#define IMGUI_LAYER_H
#include "core/layer.h"
#include "events/application_event.h"
#include "events/event.h"
#include "events/key_event.h"
#include "events/mouse_event.h"

namespace Donut
{
	class DONUT_API ImGuiLayer : public Layer
	{
	public:
		ImGuiLayer();
		~ImGuiLayer();

		virtual void onAttach() override;
		virtual void onDetach() override;
		//virtual void onUpdate() override;
		virtual void onEvent(Event& ev) override;

		virtual void onImGuiRender() override;


		void begin();
		void end();

		void setBlockEvents(bool status) { is_block_events_ = status; }

		void setDarkThemeColors();

		uint32_t getActiveWidgetID() const;
	private:
		float time_ = 0.0f;
		bool is_block_events_ = false;
	};
}

#endif