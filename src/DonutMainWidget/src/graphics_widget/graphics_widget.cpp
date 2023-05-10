#include "graphics_widget.h"

GraphicsWiget::GraphicsWiget(QWidget* parent) : QWidget(parent), Donut::Application()
{
}

GraphicsWiget::~GraphicsWiget()
{
}


Donut::Application* Donut::createApplication()
{
	return new GraphicsWiget();
	//return nullptr;
}
