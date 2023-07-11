#ifndef UI_H
#define UI_H


#include "../dependencies/imgui/imgui.h"
namespace Donut::UI
{
	struct ScopedStyleColor
	{
		ScopedStyleColor() = default;

		ScopedStyleColor(ImGuiCol idx, ImVec4 color, bool predicate = true)
			: set_(predicate)
		{
			if (predicate)
			{
				ImGui::PushStyleColor(idx, color);
			}
		}

		ScopedStyleColor(ImGuiCol idx, ImU32 color, bool predicate = true)
			: set_(predicate)
		{
			if (predicate)
			{
				ImGui::PushStyleColor(idx, color);
			}
		}

		~ScopedStyleColor()
		{
			if (set_)
			{
				ImGui::PopStyleColor();
			}
		}

	private:
		bool set_ = false;
	};
}
#endif