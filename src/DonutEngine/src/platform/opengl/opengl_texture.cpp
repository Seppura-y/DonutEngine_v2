#include "pch.h"
#include "opengl_texture.h"

#include <glad/glad.h>

namespace Donut
{
	namespace Utils 
	{
		static GLenum donutImageFormatToGLDataFormat(ImageFormat format)
		{
			switch (format)
			{
				case ImageFormat::RGB8: return GL_RGB;
				case ImageFormat::RGBA8: return GL_RGBA;
			}

			DN_CORE_ASSERT(false, "donutImageFormatToGLDataFormat : unknown format");
			return 0;
		}

		static GLenum donutImageFormatToGLInternalFormat(ImageFormat format)
		{
			switch (format)
			{
			case ImageFormat::RGB8: return GL_RGB8;
			case ImageFormat::RGBA8: return GL_RGBA8;
			}

			DN_CORE_ASSERT(false, "donutImageFormatToGLDataFormat : unknown format");
			return 0;
		}

	}

	OpenGLTexture2D::OpenGLTexture2D(const TextureSpecification& spec, Buffer data)
		: texture_spec_(spec), width_(spec.width_), height_(spec.height_)
	{
		DN_PROFILE_FUNCTION();

		internal_format_ = Utils::donutImageFormatToGLInternalFormat(texture_spec_.format_);
		data_format_ = Utils::donutImageFormatToGLDataFormat(texture_spec_.format_);

		glCreateTextures(GL_TEXTURE_2D, 1, &object_id_);
		glTextureStorage2D(object_id_, 1, internal_format_, width_, height_);

		glTextureParameteri(object_id_, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTextureParameteri(object_id_, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glTextureParameteri(object_id_, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTextureParameteri(object_id_, GL_TEXTURE_WRAP_T, GL_REPEAT);

		if(data.size_)
			setData(data);
	}

	OpenGLTexture2D::~OpenGLTexture2D()
	{
		DN_PROFILE_FUNCTION();

		glDeleteTextures(1, &object_id_);
	}

	void OpenGLTexture2D::setData(Buffer data)
	{
		DN_PROFILE_FUNCTION();

		uint32_t bytes_per_pixel = data_format_ == GL_RGBA ? 4 : 3;
		DN_CORE_ASSERT(data.size_ == width_ * height_ * bytes_per_pixel, "data must be entire texture!");
		glTextureSubImage2D(object_id_, 0, 0, 0, width_, height_, data_format_, GL_UNSIGNED_BYTE, data.data_);
		is_loaded_ = true;
	}

	uint32_t OpenGLTexture2D::getObjectId() const
	{
		return object_id_;
	}

	void OpenGLTexture2D::bind(uint32_t slot) const
	{
		DN_PROFILE_FUNCTION();

		glBindTextureUnit(slot, object_id_);
	}


}
