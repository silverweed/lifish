#pragma once
/**
 * The player
 */

#include <SFML/Graphics.hpp>
#include "MovingEntity.hpp"
#include "Lifed.hpp"

namespace Game {

class Player : public Game::MovingEntity, public Game::Lifed {
	constexpr static unsigned short MAX_LIFE = 16;
	constexpr static unsigned short WALK_N_FRAMES = 8;
	constexpr static unsigned short DEATH_N_FRAMES = 3;
public:
	Player(sf::Vector2f pos, const unsigned short id);
};

}
