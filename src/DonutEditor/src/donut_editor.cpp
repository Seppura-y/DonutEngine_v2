#include <donut.h>
#include <core/entry_point.h>

#include "editor_layer.h"

namespace Donut
{
	class DonutEditor : public Donut::Application
	{
	public:

		DonutEditor(ApplicationCommandLineArgs args)
			: Application("Donut Editor", args)
		{
			pushLayer(new Donut::EditorLayer());
		}

		~DonutEditor()
		{

		}

		void windowUpdate() override {}
	};

}


Donut::Application* Donut::createApplication(ApplicationCommandLineArgs args)
{
	return new Donut::DonutEditor(args);
}