#pragma once

#include "Boss.hpp"
#include "game.hpp"

namespace Game {

class AlienBoss : public Game::Boss {
	static const sf::Vector2i SIZE;

	void _kill();
public:
	explicit AlienBoss(const sf::Vector2f& pos);
};

}
