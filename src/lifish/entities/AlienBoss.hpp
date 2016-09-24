#pragma once

#include "Boss.hpp"
#include "Clock.hpp"
#include "FreeSighted.hpp"
#include "Shooting.hpp"
#include "AutoShooting.hpp"
#include "game.hpp"

namespace Game {

class AlienBoss : public Game::Boss {
	static const sf::Vector2i SIZE;

	unsigned short shotsFired = 0;
	Game::Clock *shootClock = nullptr;
	Game::Shooting *shooting = nullptr;
	Game::AutoShooting *autoShooting = nullptr;
	std::array<Game::FreeSighted*, 2> eyes;

	void _kill();
	void _shoot();
public:
	explicit AlienBoss(const sf::Vector2f& pos);

	void update() override;
};

}
