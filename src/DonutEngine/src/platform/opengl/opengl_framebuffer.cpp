#include "pch.h"
#include "opengl_framebuffer.h"

#include <glad/glad.h>

namespace Donut
{
	OpenGLFramebuffer::OpenGLFramebuffer(const FramebufferSpecification& spec)
		: specification_(spec)
	{
		invalidate();
	}

	OpenGLFramebuffer::~OpenGLFramebuffer()
	{
		glDeleteFramebuffers(1, &object_id_);
	}

	const FramebufferSpecification& OpenGLFramebuffer::getSpecification() const
	{
		return specification_;
	}

	void OpenGLFramebuffer::invalidate()
	{
		glCreateFramebuffers(1, &object_id_);
		glBindFramebuffer(GL_FRAMEBUFFER, object_id_);

		glCreateTextures(GL_TEXTURE_2D, 1, &color_attachment_);
		glBindTexture(GL_TEXTURE_2D, color_attachment_);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, specification_.width_, specification_.height_, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, color_attachment_, 0);

		glCreateTextures(GL_TEXTURE_2D, 1, &depth_attachment_);
		glBindTexture(GL_TEXTURE_2D, depth_attachment_);
		//glTexStorage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, specification_.width_, specification_.height_);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, specification_.width_, specification_.height_, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, depth_attachment_, 0);

		DN_CORE_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "Framebuffer is incomplete");

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void OpenGLFramebuffer::bind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, object_id_);
	}

	void OpenGLFramebuffer::unBind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
}