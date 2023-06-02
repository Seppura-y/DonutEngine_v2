#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include "core/core.h"

namespace Donut
{
	struct FramebufferSpecification
	{
		uint32_t width_, height_;
		uint32_t samples_ = 1;

		bool swapchain_target_ = false;
	};

	class Framebuffer
	{
	public:
		virtual const FramebufferSpecification& getSpecification() const = 0;
		static Ref<Framebuffer> createFramebuffer(const FramebufferSpecification& spec);

		virtual void bind() = 0;
		virtual void unBind() = 0;

		virtual void resize(uint32_t width, uint32_t height) = 0;
		
		virtual uint32_t getColorAttachmentID() const = 0;
	private:

	};
}
#endif