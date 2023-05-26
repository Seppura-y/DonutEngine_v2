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

		GLenum internal_format = 0, data_format = 0;
		if (channels == 3)
		{
			internal_format = GL_RGB8;
			data_format = GL_RGB;
		}
		else if (channels == 4)
		{
			internal_format = GL_RGBA8;
			data_format = GL_RGBA;
		}
		DN_CORE_ASSERT(internal_format & data_format, "image format is not supported!");

		width_ = width;
		height_ = height;
		channels_ = channels;

		glCreateTextures(GL_TEXTURE_2D, 1, &object_id_);
		glTextureStorage2D(object_id_, 1, internal_format, width_, height_);

		glTextureParameteri(object_id_, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTextureParameteri(object_id_, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glTextureParameteri(object_id_, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTextureParameteri(object_id_, GL_TEXTURE_WRAP_T, GL_REPEAT);

		glTextureSubImage2D(object_id_, 0, 0, 0, width_, height_, data_format, GL_UNSIGNED_BYTE, data);

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
