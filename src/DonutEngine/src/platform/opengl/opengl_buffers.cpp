#include "pch.h"
#include "opengl_buffers.h"

#include <glad/glad.h>

namespace Donut
{
	///////////////////////////////////////////////////////////////////////////////////////////////
	////Vertex Buffer
	OpenGLVertexBuffer::OpenGLVertexBuffer(float* vertices, uint32_t size)
	{
		DN_PROFILE_FUNCTION();

		glCreateBuffers(1, &object_id_);
		glBindBuffer(GL_ARRAY_BUFFER, object_id_);
		glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);
	}

	OpenGLVertexBuffer::~OpenGLVertexBuffer()
	{
		DN_PROFILE_FUNCTION();

		glDeleteBuffers(1, &object_id_);
	}

	void OpenGLVertexBuffer::setLayout(const BufferLayout& layout)
	{
		layout_ = layout;
	}

	const BufferLayout& OpenGLVertexBuffer::getLayout() const
	{
		return layout_;
	}

	void OpenGLVertexBuffer::bind() const
	{
		DN_PROFILE_FUNCTION();

		glBindBuffer(GL_ARRAY_BUFFER, object_id_);
	}

	void OpenGLVertexBuffer::unBind() const
	{
		DN_PROFILE_FUNCTION();

		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}












	///////////////////////////////////////////////////////////////////////////////////////////////
	////Index Buffer
	OpenGLIndexBuffer::OpenGLIndexBuffer(uint32_t* indices, uint32_t count)
		: indices_count_(count)
	{
		glCreateBuffers(1, &object_id_);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, object_id_);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(uint32_t), indices, GL_STATIC_DRAW);
	}

	OpenGLIndexBuffer::~OpenGLIndexBuffer()
	{
		glDeleteBuffers(1, &object_id_);
	}

	void OpenGLIndexBuffer::bind() const
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, object_id_);
	}

	void OpenGLIndexBuffer::unBind() const
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
	uint32_t OpenGLIndexBuffer::getIndicesCount() const
	{
		return indices_count_;
	}
}