#pragma once

#include "BreakableWall.hpp"

namespace Game {

/**
 * A Wall that blocks enemies and players, but not bullets.
 */
class TransparentWall : public Game::BreakableWall {
public:
	TransparentWall(const sf::Vector2f& pos);
};

}
