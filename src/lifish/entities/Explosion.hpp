#pragma once

#include <array>
#include <SFML/Graphics.hpp>
#include "Animated.hpp"
#include "Collider.hpp"

namespace Game {

class LevelManager;
class Player;

/**
 * A bomb's explosion; like in original BOOM, the explosion
 * doesn't propagate in time: rather, it blossoms in all involved
 * tiles at once and has a duration of ~200 ms (framerate: 0.05)
 */
class Explosion : public Game::Entity, public sf::Drawable {
	sf::Texture explosionHTexture,
		    explosionVTexture;

	Game::Animated *explosionC = nullptr, // central explosion
	               *explosionH = nullptr, // horizontal explosion
		       *explosionV = nullptr; // vertical explosion

	/** Compound collider used to check explosion's hits */
	Game::Collider *explCollider = nullptr;

	/** The radius of this explosion */
	unsigned short radius;

	/** The tiles involved in this explosion (valid after calling propagate());
	 *  more specifically, this is a 4-element array containing the propagation
	 *  distance in the 4 directions. The array is indexed with ANIM_UP etc
	 */
	std::array<unsigned short, 4> propagation;

	/** The player who dropped the bomb this explosion originated from */
	const Game::Player *const sourcePlayer;


	/** To be called after `propagate()`; sets the correct positions for explosionH/V */
	void _setPropagatedAnims();

public:
	/** If sourcePlayer == nullptr, the explosion wasn't originated by a bomb.
	 *  In this case, whenever points should be given to a player, no points
	 *  are given at all.
	 */
	explicit Explosion(const sf::Vector2f& pos, unsigned short radius, 
			const Game::Player *const sourcePlayer = nullptr);

	/** Calculate the tiles this explosion propagates to and fill `propagation`
	 *  (fixed walls and borders stop the explosion). Also kills enemies and
	 *  walls within the explosion. (called once in the Explosion's lifetime).
	 *  @return self
	 */
	Game::Explosion* propagate(Game::LevelManager& lm);

	void draw(sf::RenderTarget& window, sf::RenderStates states) const override;

	const Game::Player* getSourcePlayer() const { return sourcePlayer; }

	/** Hits any entity involved in this explosion */
	void checkHit(Game::LevelManager& lm);
};

}
