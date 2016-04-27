#pragma once

#include "Component.hpp"

namespace Game {

class Collider : public Game::Component {
	bool colliding = false;

public:
	explicit Collider(Game::Entity *const owner);

	bool isColliding() const { return colliding; }

	void update() override;
};

}
