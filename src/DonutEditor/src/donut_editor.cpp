#include <donut.h>
#include <core/entry_point.h>

#include "editor_layer.h"

namespace Donut
{
	class DonutEditor : public Donut::Application
	{
	public:

		DonutEditor(const ApplicationSpecification& spec)
			: Application(spec)
		{
			pushLayer(new Donut::EditorLayer());
		}

		~DonutEditor()
		{

		}

		void windowUpdate() override {}
	};

}


Donut::Application* Donut::createApplication(const ApplicationSpecification& spec)
{
	return new Donut::DonutEditor(spec);
}