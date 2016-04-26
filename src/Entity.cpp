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
