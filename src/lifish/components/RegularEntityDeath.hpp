#pragma once

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

public:
	explicit RegularEntityDeath(Game::Entity& owner, sf::Time deathTime);

	void kill();
	bool isKillInProgress() const;

	Game::Entity* init() override;
};

}
