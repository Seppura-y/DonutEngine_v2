#ifndef QGL_RENDERER_H
#define QGL_RENDERER_H

#include <mutex>
#include <QOpenGLWidget>
#include <QOpenGLFunctions_4_5_Core>
#include <QGLShaderProgram>

#include "media_types.h"
#include "ivideo_renderer.h"

struct AVFrame;
struct AVCodecParameters;

class QGLRenderer : public QOpenGLWidget, public QOpenGLFunctions_4_5_Core, public IVideoRenderer
{
	Q_OBJECT

public:
	QGLRenderer(QWidget* parent = nullptr);
	~QGLRenderer() noexcept;


	virtual int initRenderer(int width, int height, PixFormat fmt = PixFormat::PIX_FORMAT_RGBA, void* win_id = nullptr) override;
	//virtual int initRenderer(AVCodecParameters* param);
	virtual void destroyRenderer() override;
	virtual bool isRendererExit() override;


	virtual int renderPackedData(const uint8_t* data, int linesize = 0) override;
	virtual int renderYuvPlanarData(const uint8_t* y_buffer, int y_size, const uint8_t* u_buffer, int u_size, const uint8_t* v_buffer, int v_size) override;
	//virtual int renderFrame(AVFrame* frame);

protected:
	void paintGL() override;
	void initializeGL() override;
	void resizeGL(int width, int height) override;
private:
	//QGLShaderProgram program_;
	GLuint shader_program_ = 0;
	GLuint uniforms_[3] = { 0 };
	GLuint textures_[3] = { 0 };

	GLuint vao_;
	GLuint vbo_;

	unsigned char* tex_buffers_[3] = { 0 };

};

#endif