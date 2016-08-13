#pragma once

#include "Component.hpp"
#include "Animated.hpp"
#include "AxisMoving.hpp"

namespace Game {

/** 
 * A Component which animates an Animated entity based on its direction.
 */
class MovingAnimator : public Game::Component {
	
	bool active = true;

	Game::AxisMoving *moving = nullptr;
	Game::Animated *animated = nullptr;
	Game::Direction prev = Game::Direction::NONE;

public:
	explicit MovingAnimator(Game::Entity& owner);
	explicit MovingAnimator(Game::Entity& owner, Game::AxisMoving *m, Game::Animated *a);

	Game::Entity* init() override;
	void update() override;

	void setActive(bool a) { active = a; }
};

}
