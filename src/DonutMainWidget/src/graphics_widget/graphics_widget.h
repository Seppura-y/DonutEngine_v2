#ifndef GRAPHICS_WIDGET_H
#define GRAPHICS_WIDGET_H

#include <QWidget>
//#include "donut.h"
#include "DonutEngine/donut.h"

class GraphicsWiget : public QWidget, public Donut::Application
{
	Q_OBJECT

public:
	GraphicsWiget(QWidget* parent = nullptr);
	~GraphicsWiget();
private:

};


#endif