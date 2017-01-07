#pragma once

#include <SFML/Graphics.hpp>
#include "Component.hpp"

namespace lif {

class Animated;
class MovingAnimator;

/**
 * The harmless Extra-game alien sprite which is drawn
 * on screen instead of the enemies during Extra game.
 * Each Enemy object has a private pointer to this class,
 * whose draw() method is called by them instead of their own
 * when they're morphed. Note that this class by itself isn't
 * an Entity, thus cannot be used standalone.
 */
class AlienSprite : public lif::Component {
	constexpr static unsigned short WALK_N_FRAMES = 4;
	constexpr static unsigned short DEATH_N_FRAMES = 2;

	lif::Animated *animated = nullptr;
	lif::MovingAnimator *movingAnimator = nullptr;

public:
	explicit AlienSprite(lif::Entity& owner);

	lif::Entity* init() override;
};

}
