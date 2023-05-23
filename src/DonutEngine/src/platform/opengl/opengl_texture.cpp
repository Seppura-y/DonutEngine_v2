#include "pch.h"
#include "opengl_texture.h"

#include "stb_image.h"

#include <glad/glad.h>

namespace Donut
{
	OpenGLTexture2D::OpenGLTexture2D(const std::string& path)
		: file_path_(path)
	{
		stbi_set_flip_vertically_on_load(1);
		int width, height, channels;
		stbi_uc* data = stbi_load(file_path_.c_str(), &width, &height, &channels, 0);
		DN_CORE_ASSERT(data, "failed to load image");

		width_ = width;
		height_ = height;

		glCreateTextures(GL_TEXTURE_2D, 1, &object_id_);
		glTextureStorage2D(object_id_, 1, GL_RGB8, width_, height_);

		glTextureParameteri(object_id_, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTextureParameteri(object_id_, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glTextureSubImage2D(object_id_, 0, 0, 0, width_, height_, GL_RGB, GL_UNSIGNED_BYTE, data);

		stbi_image_free(data);
	}

	OpenGLTexture2D::~OpenGLTexture2D()
	{
		glDeleteTextures(1, &object_id_);
	}

	void OpenGLTexture2D::bind(uint32_t slot) const
	{
		glBindTextureUnit(slot, object_id_);
	}
}
