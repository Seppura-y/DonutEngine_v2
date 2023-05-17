#include "graphics_widget.h"

#include <QCoreApplication>
#include <QTimer>

#include <glfw/glfw3.h>

GraphicsWiget::GraphicsWiget(QWidget* parent) : QWidget(parent), Donut::Application()
{
	DN_ENGINE_INFO("{0}", "GraphicsWiget construct");

	timer_id_ = startTimer(120);

	render_widget_ = new QWidget(this);
	render_widget_->setGeometry(10, 10, this->width() - 10, this->height() - 10);
	render_widget_->setStyleSheet("background-color:black;");
	WId id = render_widget_->winId();

	Donut::WindowProps props;
	props.width = render_widget_->width();
	props.height = render_widget_->height();
	props.title = "sdf";
	props.window = id;

	//initWindow(props);

	if (!glfwInit())
	{
		DN_ENGINE_ERROR("{0}", "GLFW init failed");
		return;
	}

	GLFWwindow* win = glfwCreateWindow(800, 600, "sdf", nullptr, (GLFWwindow*)&id);
	glfwMakeContextCurrent(win);
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
		//this->update();
	}
}

void GraphicsWiget::timerEvent(QTimerEvent* ev)
{
	this->is_timeout_ = true;
	//DN_ENGINE_INFO("{0}", "timer event");
}

//
//Donut::Application* Donut::createApplication()
//{
//	return new GraphicsWiget();
//}
