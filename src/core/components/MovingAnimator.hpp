#pragma once

#include "Component.hpp"
#include "Direction.hpp"

namespace lif {

class AxisMoving;
class Animated;

/**
 * A Component which animates an Animated entity based on its direction.
 */
class MovingAnimator : public lif::Component {

	lif::AxisMoving *moving = nullptr;
	lif::Animated *animated = nullptr;
	lif::Direction prev = lif::Direction::NONE;

public:
	explicit MovingAnimator(lif::Entity& owner);
	explicit MovingAnimator(lif::Entity& owner, lif::AxisMoving *m, lif::Animated *a);

	lif::Entity* init() override;
	void update() override;
};

}
