#pragma once

#include <array>
#include "Temporary.hpp"

namespace Game {

class LevelRenderer;
class Player;

/**
 * A bomb's explosion; like in original BOOM, the explosion
 * doesn't propagate in time: rather, it blossoms in all involved
 * tiles at once and has a duration of ~200 ms (framerate: 0.05)
 */
class Explosion : public Game::Temporary {
	/** The additional sprites for horizontal and vertical
	 *  explosions (the central one is animatedSprite)
	 */
	AnimatedSprite explosionH, explosionV;

	/** The radius of this explosion */
	unsigned short radius;

	/** The tiles involved in this explosion (valid after calling propagate());
	 *  more specifically, this is a 4-element array containing the propagation
	 *  distance in the 4 directions. The array is indexed with ANIM_UP etc
	 */
	std::array<unsigned short, 4> propagation;

	/** The player who dropped the bomb this explosion originated from */
	const Game::Player *const sourcePlayer;
public:
	/** If sourcePlayer == nullptr, the explosion wasn't originated by a bomb.
	 *  In this case, whenever points should be given to a player, no points
	 *  are given at all.
	 */
	Explosion(const sf::Vector2f& pos, unsigned short radius, 
			const Game::Player *const sourcePlayer = nullptr);

	/** Calculate the tiles this explosion propagates to
	 *  (fixed walls and borders stop the explosion). Also kills enemies and
	 *  walls within the explosion.
	 */
	void propagate(LevelRenderer *const);

	void checkHit(LevelRenderer *const);

	void draw(sf::RenderTarget& window) override;
	
	void setOrigin(const sf::Vector2f& pos) override {
		animatedSprite.setOrigin(pos);
		explosionH.setOrigin(pos);
		explosionV.setOrigin(pos);
	}
};

}
