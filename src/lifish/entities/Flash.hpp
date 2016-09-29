#pragma once

#include <SFML/System.hpp>
#include "Entity.hpp"

namespace Game {

class Animated;

/**
 * The flash made by a Teleport
 */
class Flash : public Game::Entity {
	Game::Animated *animated = nullptr;

public:
	explicit Flash(const sf::Vector2f& pos);
};

}
