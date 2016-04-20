#pragma once

#include "BreakableWall.hpp"

namespace Game {

/**
 * A Wall that blocks enemies and players, but not bullets.
 */
class TransparentWall : public Game::BreakableWall {
public:
	constexpr static unsigned int SCORE = 20;
	explicit TransparentWall(const sf::Vector2f& pos);
};

}
