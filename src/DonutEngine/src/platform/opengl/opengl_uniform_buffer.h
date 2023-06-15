#ifndef OPENGL_UNIFORM_BUFFER_H
#define OPENGL_UNIFORM_BUFFER_H

#include "renderer/uniform_buffer.h"

namespace Donut
{
	class OpenGLUniformBuffer : public UniformBuffer
	{
	public:
		OpenGLUniformBuffer(uint32_t size, uint32_t binding);
		virtual ~OpenGLUniformBuffer();

		virtual void setData(const void* data, uint32_t size, uint32_t offset = 0) override;

	private:
		uint32_t object_id_ = 0;
	};
}

#endif