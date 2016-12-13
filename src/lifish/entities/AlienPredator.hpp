#pragma once

#include <SFML/System.hpp>
#include "Enemy.hpp"

namespace Game {

class AlienPredator : public Game::Enemy {
	const static sf::Time POND_LIFETIME;

public:
	explicit AlienPredator(const sf::Vector2f& pos, const Game::EnemyInfo& info);
};

}
