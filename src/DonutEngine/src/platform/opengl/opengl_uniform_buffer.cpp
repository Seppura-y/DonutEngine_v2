#include "pch.h"
#include "opengl_uniform_buffer.h"

#include <glad/glad.h>

namespace Donut
{
	OpenGLUniformBuffer::OpenGLUniformBuffer(uint32_t size, uint32_t binding)
	{
		glCreateBuffers(1, &object_id_);
		glNamedBufferData(object_id_, size, nullptr, GL_DYNAMIC_DRAW); // TODO: investigate usage hint
		glBindBufferBase(GL_UNIFORM_BUFFER, binding, object_id_);
	}

	OpenGLUniformBuffer::~OpenGLUniformBuffer()
	{
		glDeleteBuffers(1, &object_id_);
	}


	void OpenGLUniformBuffer::setData(const void* data, uint32_t size, uint32_t offset)
	{
		glNamedBufferSubData(object_id_, offset, size, data);
	}
}