#ifndef SUBTEXTURE_H
#define SUBTEXTURE_H

#include "texture.h"
#include <glm/glm.hpp>

namespace Donut
{
	class Subtexture
	{
	public:
		Subtexture(const Ref<Texture2D> texture, const glm::vec2& min, const glm::vec2& max);

		static Ref<Subtexture> createFromCoordinate(const Ref<Texture2D>& texture, const glm::vec2& coord, const glm::vec2& cell_size, const glm::vec2& sprite_size);

		const Ref<Texture2D>& getTexture() const;
		const glm::vec2* getTextureCoord() const;
	private:
		Ref<Texture2D> texture_src_;
		glm::vec2 texture_coord_[4];

	};
}
#endif