#pragma once

#include "Temporary.hpp"

namespace Game {

/**
 * The flash made by a Teleport
 */
class Flash : public Game::Temporary {
public:
	Flash(const sf::Vector2f& pos);
};

}
