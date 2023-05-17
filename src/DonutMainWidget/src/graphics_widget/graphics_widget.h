#ifndef GRAPHICS_WIDGET_H
#define GRAPHICS_WIDGET_H

#include <QWidget>
#include <QTimerEvent>

//#include "donut.h"
#include "donut.h"
#include "core/window.h"

class GraphicsWiget : public QWidget, public Donut::Application
{
	Q_OBJECT

public:
	GraphicsWiget(QWidget* parent = nullptr);
	~GraphicsWiget();

	virtual void windowUpdate() override;

protected:
	virtual void timerEvent(QTimerEvent* ev) override;
private:
	bool is_timeout_ = false;
	int timer_id_ = -1;

	QWidget* render_widget_ = nullptr;
};


#endif