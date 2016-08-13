#pragma once

#include <SFML/Graphics.hpp>
#include "Component.hpp"
#include "Animated.hpp"
#include "MovingAnimator.hpp"

namespace Game {

/**
 * The harmless Extra-game alien sprite which is drawn
 * on screen instead of the enemies during Extra game.
 * Each Enemy object has a private pointer to this class,
 * whose draw() method is called by them instead of their own
 * when they're morphed. Note that this class by itself isn't
 * an Entity, thus cannot be used standalone.
 */
class AlienSprite : public Game::Component {
	constexpr static unsigned short WALK_N_FRAMES = 4;
	constexpr static unsigned short DEATH_N_FRAMES = 2;

	Game::Animated *animated = nullptr;
	Game::MovingAnimator *movingAnimator = nullptr;

public:
	explicit AlienSprite(Game::Entity& owner);

	Game::Entity* init() override;
};

}
