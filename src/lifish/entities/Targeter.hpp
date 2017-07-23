#pragma once

#include "Entity.hpp"

namespace lif {

/**
 * A Targeter is used to mark a tile where a Missile is going to land.
 * Note that the Targeter itself does not know anything about the missile, so it must be
 * managed (in particular, killed) externally, for example by adding a Temporary component
 * with the proper expire condition to it.
 */
class Targeter : public lif::Entity {
public:
	explicit Targeter(const sf::Vector2f& pos);
};

}
