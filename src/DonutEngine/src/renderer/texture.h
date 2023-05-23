#ifndef TEXTURE_H
#define TEXTURE_H

#include "core/core.h"

#include <string>

namespace Donut
{
	class Texture
	{
	public:
		virtual ~Texture() = default;

		virtual uint32_t getWidth() const = 0;
		virtual uint32_t getHeight() const = 0;

		virtual void bind(uint32_t slot = 0) const = 0;
	private:

	};

	class Texture2D : public Texture
	{
	public:
		static Ref<Texture2D> createTexture(const std::string& path);
	};
}
#endif