#pragma once
/**
 * The harmless Extra-game alien sprite which is drawn
 * on screen instead of the enemies during Extra game.
 */

#include "Animated.hpp"
#include "MovingEntity.hpp"

namespace Game {

class AlienSprite : public Game::Animated {
	constexpr static unsigned short WALK_N_FRAMES = 4;
	constexpr static unsigned short DEATH_N_FRAMES = 2;

	sf::Texture texture;
public:
	AlienSprite();

	/** Draw the alien sprite in this position, facing this direction.
	 *  Direction::NONE means to draw its death frames.
	 */
	void draw(sf::RenderTarget& window, const sf::Vector2f& pos, const Game::Direction dir);
};

}
