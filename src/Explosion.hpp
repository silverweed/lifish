#pragma once
/**
 * A bomb's explosion; like in original BOOM, the explosion
 * doesn't propagate: rather, it blossoms in all involved
 * tiles at once and has a duration of ~200 ms (framerate: 0.05)
 */

#include "Temporary.hpp"

namespace Game {

class Explosion : public Game::Temporary {
	/** The additional sprites for horizontal and vertical
	 *  explosions (the central one is animatedSprite)
	 */
	AnimatedSprite explosionH, explosionV;
	unsigned short radius;
public:
	Explosion(const sf::Vector2f& pos, unsigned short radius);

	void draw(sf::RenderTarget& window) override;
};

}
