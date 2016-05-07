#pragma once

#include "Component.hpp"
#include "Clock.hpp"
#include "Bullet.hpp"
#include "Attack.hpp"
#include <string>

namespace Game {

class Shooting : public Game::Component {
protected:
	/** Used by CONTACT attack AI */
	sf::Vector2i attackAlign;
	
	Attack attack;
	bool shooting = false;

	Game::Clock<1> *rechargeClock = nullptr;
	
public:
	explicit Shooting(Game::Entity *const owner, const Attack& attack)
		: Game::Component(owner)
		, attackAlign(-1.f, -1.f)
		, attack(attack) 
	{
		rechargeClock = addComponent(new Game::Clock<1>(this));
	}

	const Attack& getAttack() const { return attack; }

	/** Creates a bullet described by `attack` and returns it. 
	 *  The callee must take care of its destruction.
	 */
	Game::Bullet* shoot();
	bool isShooting() const { return shooting; }

	bool isRecharging() const;
};

}
