#pragma once

#include "Component.hpp"

namespace lif {

class AxisMoving;

/**
 * An AxisMoving-endued Entity with a LeapingMovement will block for `blockTime` each time it aligns to
 * a new tile. This can be used to obtain a "leap" effect.
 */
class LeapingMovement : public lif::Component {

	sf::Time blockTime;

	lif::AxisMoving *moving = nullptr;

public:
	explicit LeapingMovement(lif::Entity& owner, sf::Time blockTime);

	lif::Entity* init() override;
	void update() override;
};

}
