#pragma once

#include "Animated.hpp"
#include "MovingEntity.hpp"
#include "Sounded.hpp"

namespace Game {

/**
 * The harmless Extra-game alien sprite which is drawn
 * on screen instead of the enemies during Extra game.
 * Each Enemy object has a private pointer to this class,
 * whose draw() method is called by them instead of their own
 * when they're morphed. Note that this class by itself isn't
 * an Entity, thus cannot be used standalone.
 */
class AlienSprite : public Game::Animated, public Game::Sounded {
	constexpr static unsigned short WALK_N_FRAMES = 4;
	constexpr static unsigned short DEATH_N_FRAMES = 2;

	sf::Texture texture;

	// Suppress compiler's warnings and prevent using
	// the base draw() function
	using Game::Animated::draw;
public:
	AlienSprite();

	/** Draw the alien sprite in this position, facing this direction.
	 *  Direction::NONE means to draw its death frames.
	 */
	void draw(sf::RenderTarget& window, const sf::Vector2f& pos, const Game::Direction dir);
};

}
