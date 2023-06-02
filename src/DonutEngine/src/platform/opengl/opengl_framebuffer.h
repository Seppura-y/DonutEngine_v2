#ifndef OPENGL_FRAMEBUFFER_H
#define OPENGL_FRAMEBUFFER_H

#include "renderer/framebuffer.h"

namespace Donut
{
	class OpenGLFramebuffer : public Framebuffer
	{
	public:
		OpenGLFramebuffer(const FramebufferSpecification& spec);
		virtual ~OpenGLFramebuffer();

		virtual const FramebufferSpecification& getSpecification() const override;

		void invalidate();
		virtual void resize(uint32_t width, uint32_t height) override;

		virtual void bind() override;
		virtual void unBind() override;

		virtual uint32_t getColorAttachmentID() const override { return color_attachment_; }
	private:
		uint32_t object_id_ = 0;
		uint32_t color_attachment_ = 0;
		uint32_t depth_attachment_ = 0;
		FramebufferSpecification specification_;
	};
}
#endif