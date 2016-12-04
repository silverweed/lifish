#pragma once

#include "Component.hpp"

namespace Game {

struct Attack;
class AutoShooting;
class FreeSighted;
class Shooting;

/** A ShootingPoint is a component with a sight and an AutoShooting.
 *  Can be used to give an entity multiple "weapons".
 */
class ShootingPoint : public Game::Component {
	sf::Vector2f offset;
	Game::FreeSighted *sighted = nullptr;
	Game::Shooting *shooting = nullptr;
	Game::AutoShooting *autoShooting = nullptr;

public:
	COMP_NOT_UNIQUE

	explicit ShootingPoint(Game::Entity& owner, const Game::Attack& attack, float visionRadius = -1);

	void setOffset(const sf::Vector2f& off) { offset = off; }

	/** @return The actual position of this component */
	sf::Vector2f getPosition() const override {
		return owner.getPosition() + offset;
	}
};

}
