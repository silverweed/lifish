#pragma once

#include "Attack.hpp"

namespace Game {

/**
 * This is the base abstract class for a component that performs some kind of shooting pattern with a given Attack.
 */
class ShootingPattern : public Game::Component {
protected:
	Game::Attack attack;

public:
	explicit ShootingPattern(Game::Entity& owner, Game::Attack attack)
		: Game::Component(owner)
		, attack(attack)
	{}

	virtual void update() override = 0;
};

}
