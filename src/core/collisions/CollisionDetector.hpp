#pragma once

#include <SFML/Graphics.hpp>
#ifndef RELEASE
#	include "Stats.hpp"
#endif

namespace lif {

class EntityGroup;

/**
 * Abstract class for a collision detector.
 */
class CollisionDetector {
protected:
	lif::EntityGroup& group;
	/** The rectangle defining the level boundaries */
	const sf::FloatRect levelLimit;
#ifndef RELEASE
	lif::Debug::Stats dbgStats;
#endif

public:
	explicit CollisionDetector(lif::EntityGroup& group, const sf::FloatRect& levelLimit);
	virtual ~CollisionDetector() {}

	sf::FloatRect getLevelLimit() const { return levelLimit; }

	virtual void update() = 0;

#ifndef RELEASE
	const lif::Debug::Stats& getStats() const { return dbgStats; }
#endif
};

}
