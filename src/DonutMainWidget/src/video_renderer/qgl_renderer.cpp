#include "qgl_renderer.h"

#include <iostream>
#include <thread>

extern"C"
{
#include <libavcodec/avcodec.h>
}

#include "log.h"


static const GLfloat vertices[] = {
	-1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
	 1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
	-1.0f,  1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
	 1.0f,  1.0f, 0.0f, 0.0f, 1.0f, 0.0f
};


static const char* vertex_string = R"(
	#version 450 core

	layout (location = 0) in vec4 aPos;
	layout (location = 1) in vec2 aUV;

	out vec2 outUV;

	void main()
	{
		gl_Position = proj_matrix * view_matrix * model_matrix * vec4(aPos.x, aPos.y, aPos.z, aPos.w);
		outUV = aUV;
	}
)";

static const char* fragment_string = R"(
	#version 450 core
	in vec2 outUV;

	out fragColor;
	
	uniform sampler2D textureY;
	uniform sampler2D textureU;
	uniform sampler2D textureV;
	void main()
	{
		vec3 yuv;
		vec3 rgb;
		yuv.x = texture2D(textureY, outUV).r;
		yuv.y = texture2D(textureU, outUV).r - 0.5;
		yuv.z = texture2D(textureV, outUV).r - 0.5;

		rgb = mat3( 1.0,		1.0,		1.0,
					0.0,		-0.39465,	2.03211,
					1.13983,	-0.58060,	0.0);
		rgb = rgb * yuv;

		fragColor = vec4(rgb, 1.0);
	}
)";

QGLRenderer::QGLRenderer(QWidget* parent) : QOpenGLWidget(parent)
{
}

QGLRenderer::~QGLRenderer()
{
	glDeleteProgram(shader_program_);
}

int QGLRenderer::initRenderer(int width, int height, PixFormat fmt, void* win_id)
{
	std::unique_lock<std::mutex> lock(mtx_);
	width_ = width;
	height_ = height;
	pix_fmt_ = fmt;
	window_id_ = win_id;

	delete tex_buffers_[0];
	delete tex_buffers_[1];
	delete tex_buffers_[2];
	tex_buffers_[0] = new unsigned char[width_ * height];
	tex_buffers_[1] = new unsigned char[width_ * height / 4];
	tex_buffers_[2] = new unsigned char[width_ * height / 4];

	if (textures_[0])
	{
		glDeleteTextures(3, textures_);
	}
	glGenTextures(3, textures_);

	glBindTexture(GL_TEXTURE_2D, textures_[0]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width / 2, height / 2, 0, GL_RED, GL_UNSIGNED_BYTE, 0);

	glBindTexture(GL_TEXTURE_2D, textures_[1]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width / 2, height / 2, 0, GL_RED, GL_UNSIGNED_BYTE, 0);

	glBindTexture(GL_TEXTURE_2D, textures_[2]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width / 2, height / 2, 0, GL_RED, GL_UNSIGNED_BYTE, 0);

	return 0;
}

void QGLRenderer::destroyRenderer()
{
}

bool QGLRenderer::isRendererExit()
{
	return false;
}

int QGLRenderer::renderPackedData(const uint8_t* data, int linesize)
{
	return 0;
}

int QGLRenderer::renderYuvPlanarData(const uint8_t* y_buffer, int y_size, const uint8_t* u_buffer, int u_size, const uint8_t* v_buffer, int v_size)
{
	if (!y_buffer || y_size <= 0 || !u_buffer || u_size <= 0 || !v_buffer || v_size <= 0)
	{
		std::cout << "DrawView failed : (!y_buffer || y_size <= 0 || !u_buffer || u_size || !v_buffer || v_size)" << std::endl;
		return -1;
	}

	std::unique_lock<std::mutex> lock(mtx_);

	//if (!window_ || !texture_ || !renderer_)
	//{
	//	std::cout << "DrawView failed : (!window_ || !texture_ || !renderer_)" << std::endl;
	//	return -1;
	//}

	if (width_ == 0 || height_ == 0)
	{
		std::cout << "DrawView failed : (width_ == 0 || height_ == 0)" << std::endl;
		return -1;
	}

	if (width_ == y_size)
	{
		memcpy(tex_buffers_[0], y_buffer, width_ * height_);
		memcpy(tex_buffers_[1], u_buffer, width_ * height_ / 4);
		memcpy(tex_buffers_[2], v_buffer, width_ * height_ / 4);
	}
	else
	{
		for (int i = 0; i < height_; i++)
		{
			memcpy(tex_buffers_[0] + width_ * i, y_buffer + y_size * i, width_);
		}

		for (int i = 0; i < height_ / 2; i++)
		{
			memcpy(tex_buffers_[1] + width_ / 2 * i, u_buffer + u_size * i, width_);
		}

		for (int i = 0; i < height_ / 2; i++)
		{
			memcpy(tex_buffers_[2] + width_ / 2 * i, v_buffer + v_size * i, width_);
		}

	}
	
	update();
	return 0;
}

void QGLRenderer::paintGL()
{
	std::unique_lock<std::mutex> lock(mtx_);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textures_[0]);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width_, height_, GL_RED, GL_UNSIGNED_BYTE, tex_buffers_[0]);
	glUniform1i(uniforms_[0], 0);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, textures_[1]);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width_ / 2, height_ / 2, GL_RED, GL_UNSIGNED_BYTE, tex_buffers_[1]);
	glUniform1i(uniforms_[1], 1);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, textures_[2]);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width_ / 2, height_ / 2, GL_RED, GL_UNSIGNED_BYTE, tex_buffers_[2]);
	glUniform1i(uniforms_[2], 2);

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

void QGLRenderer::initializeGL()
{
	std::unique_lock<std::mutex> lock(mtx_);
	initializeOpenGLFunctions();

	GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex_shader, 1, &vertex_string, NULL);
	glCompileShader(vertex_shader);

	GLint isCompiled = 0;
	glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &isCompiled);
	if (isCompiled == GL_FALSE)
	{
		GLint max_length = 0;
		glGetShaderiv(vertex_shader, GL_INFO_LOG_LENGTH, &max_length);

		// The max_length includes the NULL character
		std::vector<GLchar> info_log(max_length);
		glGetShaderInfoLog(vertex_shader, max_length, &max_length, &info_log[0]);

		glDeleteShader(vertex_shader);

		DN_CLIENT_ERROR("{0}", info_log.data());
		return;
	}

	GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
	glCompileShader(fragment_shader);

	glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &isCompiled);
	if (isCompiled == GL_FALSE)
	{
		GLint max_length = 0;
		glGetShaderiv(fragment_shader, GL_INFO_LOG_LENGTH, &max_length);

		// The max_length includes the NULL character
		std::vector<GLchar> info_log(max_length);
		glGetShaderInfoLog(fragment_shader, max_length, &max_length, &info_log[0]);

		glDeleteShader(fragment_shader);
		glDeleteShader(vertex_shader);

		DN_CLIENT_ERROR("{0}", info_log.data());
		return;
	}

	shader_program_ = glCreateProgram();
	glAttachShader(shader_program_, vertex_shader);
	glAttachShader(shader_program_, fragment_shader);
	glLinkProgram(shader_program_);

	// Note the different functions here: glGetProgram* instead of glGetShader*.
	GLint isLinked = 0;
	glGetProgramiv(shader_program_, GL_LINK_STATUS, (int*)&isLinked);
	if (isLinked == GL_FALSE)
	{
		GLint max_length = 0;
		glGetProgramiv(shader_program_, GL_INFO_LOG_LENGTH, &max_length);

		// The max_length includes the NULL character
		std::vector<GLchar> info_log(max_length);
		glGetProgramInfoLog(shader_program_, max_length, &max_length, &info_log[0]);

		glDeleteProgram(shader_program_);

		glDeleteShader(vertex_shader);
		glDeleteShader(fragment_shader);

		DN_CLIENT_ERROR("{0}", info_log.data());
		return;
	}

	// Always detach shaders after a successful link.
	glDetachShader(shader_program_, vertex_shader);
	glDetachShader(shader_program_, fragment_shader);

	glDeleteShader(vertex_shader);
	glDeleteShader(fragment_shader);


	glGenVertexArrays(1, &vao_);
	glBindVertexArray(vao_);

	glGenBuffers(1, &vbo_);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(4 * sizeof(float)));

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	textures_[0] = glGetUniformLocation(shader_program_, "textureY");
	textures_[1] = glGetUniformLocation(shader_program_, "textureU");
	textures_[2] = glGetUniformLocation(shader_program_, "textureV");
}

void QGLRenderer::resizeGL(int width, int height)
{
}
