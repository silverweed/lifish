#pragma once

#include <memory>
#include "Shooting.hpp"

namespace Game {

/** This component allows easy management of a Shooting entity.
 *  Instead of returning the bullet directly, the method shoot() 
 *  stores it in an internal buffer field which can be polled later.
 */
class AutoShooting : public Game::Component {
	Game::Bullet *latestShot = nullptr;
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
