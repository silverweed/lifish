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
public:
	Player(sf::Vector2f pos, const unsigned short id);
	
	void draw(sf::RenderTarget& window) override;
};

}
