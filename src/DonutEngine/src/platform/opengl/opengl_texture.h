#ifndef OPENGL_TEXTURE_H
#define OPENGL_TEXTURE_H

#include "renderer/texture.h"

namespace Donut
{
	class OpenGLTexture2D : public Texture2D
	{
	public:
		OpenGLTexture2D(const std::string& path);
		virtual ~OpenGLTexture2D();

		virtual uint32_t getWidth() const override { return width_; }
		virtual uint32_t getHeight() const override { return height_; }

		virtual void bind(uint32_t slot = 0) const override;
	private:
		uint32_t width_;
		uint32_t height_;
		uint32_t channels_;

		uint32_t object_id_;

		std::string file_path_;
	};
}

#endif