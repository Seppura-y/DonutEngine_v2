#ifndef GRAPHICS_TEST_H
#define GRAPHICS_TEST_H

#include <QWindow>

#include <QTimerEvent>
#include <QExposeEvent>
//#include "donut.h"
#include "donut.h"
#include "core/window.h"

struct	GLFWwindow;

class GraphicsTest : public QWindow, public Donut::Application
{
	Q_OBJECT

public:
	GraphicsTest(QWidget* parent = nullptr);
	~GraphicsTest();

	virtual void windowUpdate() override;
	void initializeGL();

protected:
	virtual void timerEvent(QTimerEvent* ev) override;

	void paintGL();
	void resizeGL(int width, int height);

	void resizeEvent(QResizeEvent* ev) override;
	void exposeEvent(QExposeEvent* ev) override;
private:
	bool is_glfw_init_ = false;
	bool is_timeout_ = false;
	int timer_id_ = -1;

	QWidget* render_widget_ = nullptr;

	GLFWwindow* window_ = nullptr;
};


#endif