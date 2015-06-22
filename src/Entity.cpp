#include "Entity.hpp"

using Game::Entity;

void Entity::draw(sf::RenderTarget& window) {
	sprite.setPosition(pos);
	window.draw(sprite);
}
