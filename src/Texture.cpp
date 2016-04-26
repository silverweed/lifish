#include "Texture.hpp"
#include "GameCache.hpp"
#include "Game.hpp"

using Game::Texture;

Texture::Texture(Game::Entity *const owner, const std::string& texture_name)
	: Game::Component(owner)
{
	texture = Game::cache.loadTexture(texture_name);

	// TODO: define texture rect
}
