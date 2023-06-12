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

		virtual uint32_t getColorAttachmentID(uint32_t index = 0) const override
		{
			DN_CORE_ASSERT((index < color_attachments_.size()),"");
			return color_attachments_[index];
		}
	private:
		uint32_t object_id_ = 0;
		
		std::vector<FramebufferTextureSpecification> color_attachment_specifications_;
		FramebufferTextureSpecification depth_attachment_specification_ = FramebufferTextureFormat::None;

		std::vector<uint32_t> color_attachments_;
		uint32_t depth_attachment_ = 0;

		FramebufferSpecification specification_;
	};
}
#endif