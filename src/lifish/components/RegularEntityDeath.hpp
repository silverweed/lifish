#pragma once

#include <unordered_map>
#include "Component.hpp"
#include "Clock.hpp"
#include "Killable.hpp"

namespace Game {

/** A generic component that handles the typical death of a Killable entity.
 *  In particular, it handles the case where owner has an Animated, an AxisMoving,
 *  a MovingAnimator or a Sounded component.
 */
class RegularEntityDeath : public Game::Component {
	const sf::Time deathTime;
	Game::Killable *killable = nullptr;
	std::unordered_map<Game::Component*, bool> origAutoRealign;

public:
	explicit RegularEntityDeath(Game::Entity& owner, sf::Time deathTime);

	void kill();
	bool isKillInProgress() const;
	/** Restores all owner's components altered by kill() */
	void resurrect();

	Game::Entity* init() override;
};

}
