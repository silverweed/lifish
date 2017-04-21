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
	sf::FloatRect levelLimit;
#ifndef RELEASE
	lif::debug::Stats dbgStats;
#endif

public:
	explicit CollisionDetector(lif::EntityGroup& group,
				const sf::FloatRect& levelLimit = sf::FloatRect(0, 0, 0, 0));
	virtual ~CollisionDetector() {}

	virtual void setLevelLimit(const sf::FloatRect& limit) { levelLimit = limit; }
	sf::FloatRect getLevelLimit() const { return levelLimit; }

	virtual void update() = 0;

#ifndef RELEASE
	const lif::debug::Stats& getStats() const { return dbgStats; }
#endif
};

}
