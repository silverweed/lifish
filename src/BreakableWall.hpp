#pragma once
/**
 * A breakable wall
 */

#include <SFML/Graphics.hpp>
#include "Wall.hpp"
#include "Animated.hpp"

namespace Game {

class BreakableWall : public Game::Wall, Game::Animated {
protected:
	unsigned short life;
public:
	BreakableWall(sf::Vector2f pos, const std::string& texture_name, unsigned short life = 1);

	virtual void draw(sf::RenderTarget& window);
};

}
