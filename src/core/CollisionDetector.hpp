#pragma once

#include "EntityGroup.hpp"

namespace Game {

class CollisionDetector final {
	Game::EntityGroup& group;

public:
	explicit CollisionDetector(Game::EntityGroup& group) 
		: group(group) {}

	void update();
};

}
