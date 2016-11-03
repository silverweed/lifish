#pragma once

#include <array>
#include "Boss.hpp"
#include "game.hpp"

namespace Game {

class Clock;
class ShootingPoint;

class AlienBoss : public Game::Boss {
	static const sf::Vector2i SIZE;

	unsigned short shotsFired = 0;
	Game::Clock *shootClock = nullptr;
	std::array<Game::ShootingPoint*, 2> eyes;

	void _shoot();
public:
	explicit AlienBoss(const sf::Vector2f& pos);

	void update() override;
};

}
