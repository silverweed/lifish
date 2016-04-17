#pragma once

#include <SFML/System.hpp>
#include "Component.hpp"

namespace Game {

/**
 * Represents a position. In order to properly work, no Component
 * other than the root entity should have an actual Position, and
 * all its component should refer to that one, so the position can
 * always be syncronized between all Components. 
 * e.g.
 * Entity my_entity;
 * my_entity.addComponent(new Position<float>(my_entity, sf::Vector2f(0, 0)));
 * my_entity.addComponent(new Moving(my_entity)); // uses my_entity.get<Position>().
 */
template <typename T>
class Position : public Game::Component {
	sf::Vector2<T> position;

public:
	Position(Game::Entity *const owner, const sf::Vector2<T>& pos)
		: Game::Component(owner)
	{
		position = pos;
	}

	void set(const sf::Vector2<T>& pos) {
		position = pos;
	}
};

}
