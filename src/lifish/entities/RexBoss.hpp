#pragma once

#include "Boss.hpp"

namespace lif {

class AxisMoving;

class RexBoss : public lif::Boss {
	bool wasBlocked = false;

	lif::AxisMoving *moving = nullptr;

public:
	explicit RexBoss(const sf::Vector2f& pos);

	void update() override;
};

}
