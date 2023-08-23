#include "pch.h"
#include "texture.h"

#include "renderer/renderer.h"
#include "platform/opengl/opengl_texture.h"

namespace Donut
{
	Ref<Texture2D> Texture2D::createTexture(const TextureSpecification& spec, Buffer data)
	{
		switch (RendererAPI::getCurrentAPIType())
		{
			case RendererAPI::RendererAPIType::None: DN_CORE_ASSERT(false, "RendererAPI::RendererAPIType::None:"); return nullptr;
			case RendererAPI::RendererAPIType::OpenGL: return createRef<OpenGLTexture2D>(spec, data);
		}
		DN_CORE_ASSERT(false, "unknown renderer api");

		return nullptr;
	}
}