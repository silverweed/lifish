#pragma once

#include <array>
#include "Boss.hpp"
#include "game.hpp"

namespace lif {

class Clock;
class ShootingPoint;

class AlienBoss : public lif::Boss {
	unsigned short shotsFired = 0;
	lif::Clock *shootClock = nullptr;
	std::array<lif::ShootingPoint*, 2> eyes;

public:
	explicit AlienBoss(const sf::Vector2f& pos);

	void update() override;
};

}
