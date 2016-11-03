#pragma once

namespace Game {

class EntityGroup;

/**
 * Abstract class for a collision detector.
 */
class CollisionDetector {
protected:
	Game::EntityGroup& group;

public:
	explicit CollisionDetector(Game::EntityGroup& group);

	virtual void update() = 0;
};

}
