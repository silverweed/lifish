#pragma once

#include "Boss.hpp"
#include "game.hpp"
#include <array>

namespace lif {

class Animated;
class ShootingPoint;

class AlienBoss : public lif::Boss {
	lif::Animated *animated = nullptr;
	std::array<lif::ShootingPoint*, 2> eyes;

	bool canShoot = false;
	sf::Time shootT;

public:
	explicit AlienBoss(const sf::Vector2f& pos);

	void update() override;
};

}
