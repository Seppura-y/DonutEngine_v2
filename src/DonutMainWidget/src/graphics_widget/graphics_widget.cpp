#include "graphics_widget.h"


GraphicsWiget::GraphicsWiget(QWidget* parent) : QWidget(parent), Donut::Application()
{
	DN_ENGINE_INFO("{0}", "SDF");
}

GraphicsWiget::~GraphicsWiget()
{
}


Donut::Application* Donut::createApplication()
{
	return new GraphicsWiget();
	//return nullptr;
}
