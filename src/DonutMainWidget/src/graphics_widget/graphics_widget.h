#ifndef GRAPHICS_WIDGET_H
#define GRAPHICS_WIDGET_H

#include <QWidget>
#include "donut.h"

class GraphicsWiget : public QWidget, public Donut::Application
{
	Q_OBJECT

public:

private:

};

Donut::Application* Donut::createApplication()
{
	return new GraphicsWiget();
}

#endif