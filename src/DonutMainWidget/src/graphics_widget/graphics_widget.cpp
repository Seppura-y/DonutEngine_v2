#include "graphics_widget.h"

#include <QCoreApplication>
#include <QTimer>

GraphicsWiget::GraphicsWiget(QWidget* parent) : QWidget(parent), Donut::Application()
{
	DN_ENGINE_INFO("{0}", "GraphicsWiget construct");

	timer_id_ = startTimer(120);
	WId id = winId();
}

GraphicsWiget::~GraphicsWiget()
{
}

void GraphicsWiget::windowUpdate()
{
	QCoreApplication::processEvents();
	if (is_timeout_)
	{
		is_timeout_ = false;
		this->update();
	}
}

void GraphicsWiget::timerEvent(QTimerEvent* ev)
{
	this->is_timeout_ = true;
	DN_ENGINE_INFO("{0}", "timer event");
}


Donut::Application* Donut::createApplication()
{
	return new GraphicsWiget();
}
