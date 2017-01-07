#pragma once

#include <SFML/Graphics.hpp>
#ifndef RELEASE
#	include "Stats.hpp"
#endif

namespace Game {

class EntityGroup;

/**
 * Abstract class for a collision detector.
 */
class CollisionDetector {
protected:
	Game::EntityGroup& group;
	/** The rectangle defining the level boundaries */
	const sf::FloatRect levelLimit;
#ifndef RELEASE
	Game::Debug::Stats dbgStats;
#endif

public:
	explicit CollisionDetector(Game::EntityGroup& group, const sf::FloatRect& levelLimit);
	virtual ~CollisionDetector() {}

	virtual void update() = 0;

#ifndef RELEASE
	const Game::Debug::Stats& getStats() const { return dbgStats; }
#endif
};

}
