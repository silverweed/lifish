#pragma once

#include "Shooting.hpp"

namespace Game {

/**
 * This is the base abstract class for a component that drives the attacking pattern of a Shooting component.
 */
class ShootingPattern : public Game::Component {
protected:
	Game::Shooting& shooting;

public:
	explicit ShootingPattern(Game::Entity& owner, Game::Shooting& shooting)
		: Game::Component(owner)
		, shooting(shooting)
	{}

	virtual void update() override = 0;
};

}
