#ifndef TEXTURE_H
#define TEXTURE_H

#include "core/core.h"

#include <string>

namespace Donut
{
	enum class ImageFormat
	{
		None = 0,
		R8,
		RGB8,
		RGBA8,
		RGBA32F
	};

	struct TextureSpecification
	{
		uint32_t width_ = 1;
		uint32_t height_ = 1;
		ImageFormat format_ = ImageFormat::RGBA8;
		bool generate_mips_ = true;
	};

	class Texture
	{
	public:
		virtual ~Texture() = default;

		virtual const TextureSpecification& getSpecification() const = 0;

		virtual uint32_t getWidth() const = 0;
		virtual uint32_t getHeight() const = 0;

		virtual void setData(void* data, uint32_t size) = 0;

		virtual uint32_t getObjectId() const = 0;

		virtual const std::string& getPath() const = 0;

		virtual void bind(uint32_t slot = 0) const = 0;

		virtual bool isLoaded() const = 0;

		virtual bool operator==(const Texture& other) const = 0;
	private:

	};

	class Texture2D : public Texture
	{
	public:
		static Ref<Texture2D> createTexture(const TextureSpecification& spec);
		static Ref<Texture2D> createTexture(const std::string& path);
	};
}
#endif