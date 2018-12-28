#pragma once

#include "Boss.hpp"
#include "game.hpp"
#include <array>

namespace lif {

class Animated;
class Clock;
class ShootingPoint;

class AlienBoss : public lif::Boss {
	unsigned short shotsFired = 0;
	lif::Animated *animated = nullptr;
	lif::Clock *shootClock = nullptr;
	std::array<lif::ShootingPoint*, 2> eyes;
	bool canShoot = false;

public:
	explicit AlienBoss(const sf::Vector2f& pos);

	void update() override;
};

}
