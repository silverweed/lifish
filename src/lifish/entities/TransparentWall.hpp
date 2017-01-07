#pragma once

#include "BreakableWall.hpp"

namespace lif {

/**
 * A Wall that blocks enemies and players, but not bullets.
 */
class TransparentWall : public lif::BreakableWall {
public:
	constexpr static unsigned int SCORE = 20;
	explicit TransparentWall(const sf::Vector2f& pos);
};

}
