#pragma once
/**
 * A breakable wall
 */

#include <SFML/Graphics.hpp>
#include "Wall.hpp"
#include "Animated.hpp"
#include "Lifed.hpp"

namespace Game {

class BreakableWall : public Game::Wall, public Game::Lifed, public Game::Animated {
public:
	BreakableWall(sf::Vector2f pos, const unsigned short id, unsigned short life = 1);

	virtual void draw(sf::RenderTarget& window) override;
};

}
