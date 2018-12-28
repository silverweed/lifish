#pragma once

#include "Boss.hpp"
#include "game.hpp"
#include <array>

namespace lif {

class Animated;
class ShootingPoint;

class AlienBoss : public lif::Boss {
	unsigned short shotsFired = 0;
	lif::Animated *animated = nullptr;
	std::array<lif::ShootingPoint*, 2> eyes;

	bool canShoot = false;
	sf::Time shootTime;

public:
	explicit AlienBoss(const sf::Vector2f& pos);

	void update() override;
};

}
