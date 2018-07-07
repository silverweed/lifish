#pragma once

#include "OneShotFX.hpp"

namespace lif {

class Animated;

/**
 * The explosions appearing when a Boss dies.
 */
class BossExplosion : public lif::OneShotFX {
public:
	explicit BossExplosion(const sf::Vector2f& pos);
};

}
