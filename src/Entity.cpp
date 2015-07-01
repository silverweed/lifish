#include "Entity.hpp"
#include "GameCache.hpp"
#include "Game.hpp"

using Game::Entity;

Entity::Entity(sf::Vector2f _pos, const std::string& texture_name)
	: pos(_pos)
{
	Game::cache.loadTexture(texture, texture_name); 
	sprite.setTexture(texture);
	sprite.setPosition(pos);
}

void Entity::draw(sf::RenderTarget& window) {
	window.draw(sprite);
}
