#include "pch.h"
#include "opengl_texture.h"

#include "stb_image.h"

#include <glad/glad.h>

namespace Donut
{
	OpenGLTexture2D::OpenGLTexture2D(uint32_t width, uint32_t height)
		: width_(width), height_(height)
	{
		internal_format_ = GL_RGBA8, data_format_ = GL_RGBA;

		glCreateTextures(GL_TEXTURE_2D, 1, &object_id_);
		glTextureStorage2D(object_id_, 1, internal_format_, width_, height_);

		glTextureParameteri(object_id_, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTextureParameteri(object_id_, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glTextureParameteri(object_id_, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTextureParameteri(object_id_, GL_TEXTURE_WRAP_T, GL_REPEAT);
	}

	OpenGLTexture2D::OpenGLTexture2D(const std::string& path)
		: file_path_(path)
	{
		stbi_set_flip_vertically_on_load(1);
		int width, height, channels;
		stbi_uc* data = stbi_load(file_path_.c_str(), &width, &height, &channels, 0);
		DN_CORE_ASSERT(data, "failed to load image");

		if (channels == 3)
		{
			internal_format_ = GL_RGB8;
			data_format_ = GL_RGB;
		}
		else if (channels == 4)
		{
			internal_format_ = GL_RGBA8;
			data_format_ = GL_RGBA;
		}
		DN_CORE_ASSERT(internal_format_ & data_format_, "image format is not supported!");

		width_ = width;
		height_ = height;
		channels_ = channels;

		glCreateTextures(GL_TEXTURE_2D, 1, &object_id_);
		glTextureStorage2D(object_id_, 1, internal_format_, width_, height_);

		glTextureParameteri(object_id_, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTextureParameteri(object_id_, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glTextureParameteri(object_id_, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTextureParameteri(object_id_, GL_TEXTURE_WRAP_T, GL_REPEAT);

		glTextureSubImage2D(object_id_, 0, 0, 0, width_, height_, data_format_, GL_UNSIGNED_BYTE, data);

		stbi_image_free(data);
	}

	OpenGLTexture2D::~OpenGLTexture2D()
	{
		glDeleteTextures(1, &object_id_);
	}

	void OpenGLTexture2D::setData(void* data, uint32_t size)
	{
		uint32_t bytes_per_pixel = data_format_ == GL_RGBA ? 4 : 3;
		DN_CORE_ASSERT(size == width_ * height_ * bytes_per_pixel, "data must be entire texture!");
		glTextureSubImage2D(object_id_, 0, 0, 0, width_, height_, data_format_, GL_UNSIGNED_BYTE, data);
	}

	void OpenGLTexture2D::bind(uint32_t slot) const
	{
		glBindTextureUnit(slot, object_id_);
	}


}
