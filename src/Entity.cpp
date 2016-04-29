#include "Entity.hpp"
#include "GameCache.hpp"
#include "Game.hpp"
#include "Component.hpp"
#include <type_traits>

using Game::Entity;

void Entity::setOrigin(const sf::Vector2f& origin) {
	WithOrigin::setOrigin(origin);

	for (auto& c : components) {
		auto o = dynamic_cast<Game::WithOrigin*>(c.get());
		if (o != nullptr)
			o->setOrigin(origin);
	}
}

bool Entity::isAligned(const char axis) const {
	switch (axis) {
		case 'x': return (unsigned short)position.x % Game::TILE_SIZE == 0;
		case 'y': return (unsigned short)position.y % Game::TILE_SIZE == 0;
	}
	return (unsigned short)position.x % Game::TILE_SIZE == 0 
		&& (unsigned short)position.y % Game::TILE_SIZE == 0;
}

void Entity::update() {
	for (auto& c : components)
		c->update();
}
