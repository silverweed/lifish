#pragma once
/**
 * The flash made by a Teleport
 */

#include "Temporary.hpp"

namespace Game {

class Flash : public Game::Temporary {
public:
	Flash(const sf::Vector2f& pos);
};

}
