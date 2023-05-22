#include "pch.h"
#include "platform/opengl/opengl_vertex_array.h"

#include <glad/glad.h>
namespace Donut
{
	static GLenum shaderDataTypeToGLenumType(const ShaderDataType& type)
	{
		switch (type)
		{
			case ShaderDataType::Float:		return GL_FLOAT;
			case ShaderDataType::Float2:	return GL_FLOAT;
			case ShaderDataType::Float3:	return GL_FLOAT;
			case ShaderDataType::Float4:	return GL_FLOAT;
			case ShaderDataType::Int:		return GL_INT;
			case ShaderDataType::Int2:		return GL_INT;
			case ShaderDataType::Int3:		return GL_INT;
			case ShaderDataType::Int4:		return GL_INT;
			case ShaderDataType::Bool:		return GL_BOOL;
		}

		DN_CORE_ASSERT(false, "Unknown shader data type!");
		return 0;
	}

	OpenGLVertexArray::OpenGLVertexArray()
	{
		glCreateVertexArrays(1, &object_id_);
	}

	OpenGLVertexArray::~OpenGLVertexArray()
	{
		glDeleteVertexArrays(1, &object_id_);
	}

	void OpenGLVertexArray::addVertexBuffer(const Donut::Ref<VertexBuffer>& buffer)
	{
		DN_CORE_ASSERT(buffer->getLayout().getBufferElements().size(), "vertex buffer is empty!");

		glBindVertexArray(object_id_);
		buffer->bind();

		int index = 0;
		const auto& lay = buffer->getLayout();
		for (auto& element : lay)
		{
			glEnableVertexAttribArray(index);

			glVertexAttribPointer(index,
				element.getComponentCount(),
				shaderDataTypeToGLenumType(element.type_),
				element.normalized_ ? GL_TRUE : GL_FALSE,
				lay.getStride(),
				(const void*)element.offset_
			);

			index++;
		}
		vertex_buffers_.push_back(buffer);
	}

	void OpenGLVertexArray::setIndexBuffer(const Donut::Ref<IndexBuffer>& buffer)
	{
		glBindVertexArray(object_id_);
		index_buffer_ = buffer;
		index_buffer_->bind();
	}

	void OpenGLVertexArray::bind() const
	{
		glBindVertexArray(object_id_);
	}

	void OpenGLVertexArray::unBind() const
	{
		glBindVertexArray(0);
	}
}