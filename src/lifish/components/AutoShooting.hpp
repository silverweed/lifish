#pragma once

#include <memory>
#include <queue>
#include "Component.hpp"
#include "Direction.hpp"

namespace Game {

class Shooting;
class Bullet;

/** This component allows easy management of a Shooting entity.
 *  Instead of returning the bullet directly, the method shoot() 
 *  stores it in an internal buffer which can be polled later.
 */
class AutoShooting : public Game::Component {
	std::queue<std::unique_ptr<Game::Bullet>> latestShot;
	Game::Shooting *shooting = nullptr;

public:
	explicit AutoShooting(Game::Entity& owner);

	void shoot(Game::Direction dir = Game::Direction::NONE);
	void shoot(double angle);

	/** If this component's entity has shot a bullet since latest poll, return it
	 *  and empty the internal "bullet buffer". Else, just return nullptr.
	 *  Callee gets the ownership of the returned bullet.
	 */
	std::unique_ptr<Game::Bullet> pollShot();

	Game::Entity* init() override;
};

}
