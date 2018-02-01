#pragma once

#include "AxisBullet.hpp"

namespace lif {

class Clock;

class Rocket : public lif::AxisBullet {

	const float maxSpeed;

	lif::Clock *accelClock = nullptr;

public:
	explicit Rocket(const sf::Vector2f& pos, lif::Direction dir, const lif::BulletInfo& info,
			const lif::Entity *const source = nullptr);

	void update() override;
};

}
