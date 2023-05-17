#include "graphics_test.h"

#include <QCoreApplication>
#include <QTimer>
#include <QSurface>
#include <QWidget>
#include <QApplication>

//#include <glad/include/glad/glad.h>
#include <glfw/glfw3.h>

GraphicsTest::GraphicsTest(QWidget* parent) : Donut::Application()
{
	DN_ENGINE_INFO("{0}", "GraphicsWiget construct");

	setGeometry(100, 100, 800, 600);
	create();
	setSurfaceType(QSurface::OpenGLSurface);

	timer_id_ = startTimer(120);

}

GraphicsTest::~GraphicsTest()
{
}

void GraphicsTest::windowUpdate()
{
	QCoreApplication::processEvents();
	if (is_timeout_)
	{
		is_timeout_ = false;
		//this->update();
	}
}

void GraphicsTest::timerEvent(QTimerEvent* ev)
{
	this->is_timeout_ = true;
	paintGL();
	//DN_ENGINE_INFO("{0}", "timer event");
}

void GraphicsTest::paintGL()
{
	glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	if (window_)
	{
		glfwSwapBuffers(window_);
		DN_ENGINE_INFO("{0}", "paint");
	}
}

void GraphicsTest::initializeGL()
{
	if (!glfwInit())
	{
		is_glfw_init_ = false;
	}
	else
	{
		is_glfw_init_ = true;
	}
	WId id = this->winId();

	//if (is_glfw_init_)
	//{
	//	//int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

	//	
	//	window_ = glfwCreateWindow(800, 600, "graphics test", nullptr, nullptr);
	//	//window_ = glfwCreateWindow(800, 600, "graphics test", nullptr, (GLFWwindow*)&id);
	//	if (!window_)
	//	{
	//		DN_ENGINE_ERROR("{0}", "create window failed");
	//	}
	//	//glfwSetWin32Window(win, reinterpret_cast<HWND>(winId()));
	//	glfwMakeContextCurrent(window_);
	//	//DN_ENGINE_INFO("OpenGL Info:");
	//	//DN_ENGINE_INFO("  Vendor: {0}", (char*)glGetString(GL_VENDOR));
	//	//DN_ENGINE_INFO("  Renderer: {0}", (char*)glGetString(GL_RENDERER));
	//	//DN_ENGINE_INFO("  Version: {0}", (char*)glGetString(GL_VERSION));
	//}

	//window_ = glfwCreateWindow(800, 600, "graphcis test", nullptr, nullptr);
	window_ = glfwCreateWindow(800, 600, "graphics test", nullptr, (GLFWwindow*)&id);
	if (!window_)
	{
		DN_ENGINE_ERROR("{0}", "create window failed");
	}
	glfwMakeContextCurrent(window_);

}

void GraphicsTest::resizeGL(int width, int height)
{

}

void GraphicsTest::resizeEvent(QResizeEvent* ev)
{
	QWindow::resizeEvent(ev);

}

void GraphicsTest::exposeEvent(QExposeEvent* ev)
{
	QWindow::exposeEvent(ev);

	if (isExposed())
	{
		paintGL();
	}
}


Donut::Application* Donut::createApplication()
{
	return new GraphicsTest();
}
