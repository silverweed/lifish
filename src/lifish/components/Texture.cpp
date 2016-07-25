#include "Texture.hpp"
#include "GameCache.hpp"
#include "game.hpp"

using Game::Texture;

Texture::Texture(Game::Entity& owner, const std::string& texture_name)
	: Game::Component(owner)
{
	texture = Game::cache.loadTexture(texture_name);

	// TODO: define texture rect
}
