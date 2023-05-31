#include "pch.h"
#include "subtexture.h"

namespace Donut
{
	Subtexture::Subtexture(const Ref<Texture2D> texture, const glm::vec2& min, const glm::vec2& max)
		: texture_src_(texture)
	{
		texture_coord_[0] = { min.x, min.y };
		texture_coord_[1] = { max.x, min.y };
		texture_coord_[2] = { max.x, max.y };
		texture_coord_[3] = { min.x, max.y };
	}

	Ref<Subtexture> Subtexture::createFromCoordinate(const Ref<Texture2D>& texture, const glm::vec2& coord, const glm::vec2& cell_size, const glm::vec2& sprite_size)
	{
		glm::vec2 min = { (coord.x * sprite_size.x) / texture->getWidth(), (coord.y * sprite_size.y) / texture->getHeight() };
		glm::vec2 max = { ((coord.x + cell_size.x) * sprite_size.x) / texture->getWidth(), ((coord.y + cell_size.y)* sprite_size.y) / texture->getHeight() };
		return createRef<Subtexture>(texture, min, max);
	}

	const Ref<Texture2D>& Subtexture::getTexture() const
	{
		return texture_src_;
	}

	const glm::vec2* Subtexture::getTextureCoord() const
	{
		return texture_coord_;
	}
}