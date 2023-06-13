#include "pch.h"
#include "opengl_framebuffer.h"

#include <glad/glad.h>

namespace Donut
{
	static const uint32_t s_max_framebuffer_size = 8192;


	namespace Utils {

		static GLenum textureTarget(bool multisampled)
		{
			return multisampled ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;
		}

		static void createTextures(bool multisampled, uint32_t* outID, uint32_t count)
		{
			glCreateTextures(textureTarget(multisampled), count, outID);
		}

		static void bindTexture(bool multisampled, uint32_t id)
		{
			glBindTexture(textureTarget(multisampled), id);
		}

		static void attachColorTexture(uint32_t id, int samples, GLenum format, uint32_t width, uint32_t height, int index)
		{
			bool multisampled = samples > 1;
			if (multisampled)
			{
				glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, format, width, height, GL_FALSE);
			}
			else
			{
				glTexImage2D(
					GL_TEXTURE_2D,
					0,
					format,			//internal format (how the opengl store the data)
					width, height,
					0,
					GL_RGBA,		//access format (how the opengl access tha data)
					GL_UNSIGNED_BYTE,
					nullptr
				);

				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			}

			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + index, textureTarget(multisampled), id, 0);
		}

		static void attachDepthTexture(uint32_t id, int samples, GLenum format, GLenum attachmentType, uint32_t width, uint32_t height)
		{
			bool multisampled = samples > 1;
			if (multisampled)
			{
				glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, format, width, height, GL_FALSE);
			}
			else
			{
				glTexStorage2D(GL_TEXTURE_2D, 1, format, width, height);

				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			}

			glFramebufferTexture2D(GL_FRAMEBUFFER, attachmentType, textureTarget(multisampled), id, 0);
		}

		static bool isDepthFormat(FramebufferTextureFormat format)
		{
			switch (format)
			{
			case FramebufferTextureFormat::DEPTH24STENCIL8:  return true;
			}

			return false;
		}

	}

	OpenGLFramebuffer::OpenGLFramebuffer(const FramebufferSpecification& spec)
		: specification_(spec)
	{
		for (auto spec : specification_.attachments_specifications_.texture_specifications_)
		{
			if (!Utils::isDepthFormat(spec.texture_format_))
			{
				color_attachment_specifications_.emplace_back(spec);
			}
			else
			{
				depth_attachment_specification_ = spec;
			}
		}

		invalidate();
	}

	OpenGLFramebuffer::~OpenGLFramebuffer()
	{
		glDeleteFramebuffers(1, &object_id_);
		glDeleteTextures(color_attachments_.size(), color_attachments_.data());
		glDeleteTextures(1, &depth_attachment_);
	}

	const FramebufferSpecification& OpenGLFramebuffer::getSpecification() const
	{
		return specification_;
	}

	void OpenGLFramebuffer::invalidate()
	{
		if (object_id_)
		{
			glDeleteFramebuffers(1, &object_id_);
			glDeleteTextures(color_attachments_.size(), color_attachments_.data());
			glDeleteTextures(1, &depth_attachment_);
		}

		glCreateFramebuffers(1, &object_id_);
		glBindFramebuffer(GL_FRAMEBUFFER, object_id_);

		bool is_multisample = specification_.samples_ > 1;

		if (color_attachment_specifications_.size())
		{
			color_attachments_.resize(color_attachment_specifications_.size());
			Utils::createTextures(is_multisample, color_attachments_.data(), color_attachments_.size());

			for (size_t i = 0; i < color_attachments_.size(); i++)
			{
				Utils::bindTexture(is_multisample, color_attachments_[i]);
				switch (color_attachment_specifications_[i].texture_format_)
				{
					case FramebufferTextureFormat::RGBA8:
					{
						Utils::attachColorTexture(
							color_attachments_[i],
							specification_.samples_,
							GL_RGBA8,
							specification_.width_,
							specification_.height_,
							i
						);
						break;
					}
				}
			}
		}

		if (depth_attachment_specification_.texture_format_ != FramebufferTextureFormat::None)
		{
			Utils::createTextures(is_multisample, &depth_attachment_, 1);
			Utils::bindTexture(is_multisample, depth_attachment_);

			switch (depth_attachment_specification_.texture_format_)
			{
				case FramebufferTextureFormat::DEPTH24STENCIL8:
				{
					Utils::attachDepthTexture(
						depth_attachment_,
						specification_.samples_,
						GL_DEPTH24_STENCIL8,
						GL_DEPTH_STENCIL_ATTACHMENT,
						specification_.width_, specification_.height_
					);
					break;
				}
			}
		}

		if (color_attachments_.size() > 1)
		{
			DN_CORE_ASSERT(color_attachments_.size() <= 4, "");
			GLenum buffers[4] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
			glDrawBuffers(color_attachments_.size(), buffers);
		}
		else if (color_attachments_.empty())
		{
			// only depth
			glDrawBuffer(GL_NONE);
		}

		DN_CORE_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "Framebuffer is incomplete!");

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		//glCreateTextures(GL_TEXTURE_2D, 1, &color_attachment_);
		//glBindTexture(GL_TEXTURE_2D, color_attachment_);
		//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, specification_.width_, specification_.height_, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		//glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, color_attachment_, 0);

		//glCreateTextures(GL_TEXTURE_2D, 1, &depth_attachment_);
		//glBindTexture(GL_TEXTURE_2D, depth_attachment_);
		////glTexStorage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, specification_.width_, specification_.height_);
		//glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, specification_.width_, specification_.height_, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL);
		//glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, depth_attachment_, 0);

		//DN_CORE_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "Framebuffer is incomplete");

		//glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void OpenGLFramebuffer::resize(uint32_t width, uint32_t height)
	{
		if (width == 0 || height == 0 || width > s_max_framebuffer_size || height > s_max_framebuffer_size)
		{
			return;
		}
		specification_.width_ = width;
		specification_.height_ = height;
		invalidate();
	}

	void OpenGLFramebuffer::bind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, object_id_);
		glViewport(0, 0, specification_.width_, specification_.height_);
	}

	void OpenGLFramebuffer::unBind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
}