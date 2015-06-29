#include "Entity.hpp"
#include "GameCache.hpp"
#include "Game.hpp"

using Game::Entity;

Entity::Entity(sf::Vector2f _pos) : pos(_pos) {}

Entity::Entity(sf::Vector2f _pos, const std::string& texture_name)
	: pos(_pos)
{
	Game::cache.loadTexture(texture, texture_name); 
}

void Entity::draw(sf::RenderTarget& window) {
	sprite.setPosition(pos);
	window.draw(sprite);
}
