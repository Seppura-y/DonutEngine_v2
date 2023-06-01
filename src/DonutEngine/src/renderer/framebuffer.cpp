#include "pch.h"
#include "framebuffer.h"
#include "platform/opengl/opengl_framebuffer.h"
#include "renderer/renderer.h"

namespace Donut
{
	Ref<Framebuffer> Framebuffer::createFramebuffer(const FramebufferSpecification& spec)
	{
		switch (RendererAPI::getCurrentAPIType())
		{
		case RendererAPI::RendererAPIType::None: DN_CORE_ASSERT(false, "RendererAPI::RendererAPIType::None:"); return nullptr;
		case RendererAPI::RendererAPIType::OpenGL: return createRef<OpenGLFramebuffer>(spec);
		}
		DN_CORE_ASSERT(false, "unknown renderer api");

		return nullptr;
	}
}