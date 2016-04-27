#include "Entity.hpp"
#include "GameCache.hpp"
#include "Game.hpp"
#include "Component.hpp"
#include <type_traits>

using Game::Entity;

void Entity::setOrigin(const sf::Vector2f& origin) {
	WithOrigin::setOrigin(origin);

	// TODO
	//for (auto& c : components) {
		//if (instanceof<Game::WithOrigin>(c))
			//static_cast<Game::WithOrigin*>(c)->setOrigin(origin);
	//}
}

bool Entity::isAligned(const char axis) const {
	switch (axis) {
		case 'x': return (unsigned short)pos.x % Game::TILE_SIZE == 0;
		case 'y': return (unsigned short)pos.y % Game::TILE_SIZE == 0;
	}
	return (unsigned short)pos.x % Game::TILE_SIZE == 0 
		&& (unsigned short)pos.y % Game::TILE_SIZE == 0;
}
