#include "Entity.hpp"
#include "GameCache.hpp"
#include "Game.hpp"

using Game::Entity;

Entity::Entity(const sf::Vector2f& _pos, const std::string& texture_name)
	: pos(_pos)
{
	if (texture_name.length() > 0)
		Game::cache.loadTexture(texture, texture_name); 
}

bool Entity::isAligned(const char axis) const {
	switch (axis) {
		case 'x': return (unsigned short)pos.x % Game::TILE_SIZE == 0;
		case 'y': return (unsigned short)pos.y % Game::TILE_SIZE == 0;
	}
	return (unsigned short)pos.x % Game::TILE_SIZE == 0 
		&& (unsigned short)pos.y % Game::TILE_SIZE == 0;
}
