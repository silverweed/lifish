#include "Entity.hpp"

using Game::Entity;

void Entity::draw(sf::RenderTarget& window) {
	window.draw(sprite);
}
