#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include "core/core.h"

namespace Donut
{
	enum class FramebufferTextureFormat
	{
		None = 0,

		RGBA8,
		RED_INTEGER,
		
		DEPTH24STENCIL8,

		// default
		Depth = DEPTH24STENCIL8
	};

	struct FramebufferTextureSpecification
	{
		FramebufferTextureSpecification() = default;
		FramebufferTextureSpecification(FramebufferTextureFormat format) 
			: texture_format_(format) {}

		FramebufferTextureFormat texture_format_ = FramebufferTextureFormat::None;
	};

	struct FramebufferAttachmentSpecification
	{
		FramebufferAttachmentSpecification() = default;
		FramebufferAttachmentSpecification(std::initializer_list<FramebufferTextureSpecification> textures_specs)
			: texture_specifications_(textures_specs) {}

		std::vector<FramebufferTextureSpecification> texture_specifications_;
	};

	struct FramebufferSpecification
	{
		uint32_t width_, height_;
		uint32_t samples_ = 1;
		FramebufferAttachmentSpecification attachments_specifications_;

		bool swapchain_target_ = false;
	};

	class Framebuffer
	{
	public:
		static Ref<Framebuffer> createFramebuffer(const FramebufferSpecification& spec);


		virtual ~Framebuffer() = default;


		virtual void bind() = 0;
		virtual void unBind() = 0;

		virtual void resize(uint32_t width, uint32_t height) = 0;
		virtual int readPixel(uint32_t attachment, int x, int y) = 0;
		
		virtual const FramebufferSpecification& getSpecification() const = 0;

		virtual uint32_t getColorAttachmentID(uint32_t index = 0) const = 0;
	private:

	};
}
#endif