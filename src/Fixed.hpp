#pragma once

#include "Component.hpp"

namespace Game {

/**
 * A Fixed Entity is an entity which cannot ever move and is supposed to be
 * the sole occupier of its tile. This allows the EntityGroup to perform
 * some optimizations on this Entity.
 */
class Fixed : public Game::Component {
public:
	explicit Fixed(Game::Entity *const owner)
		: Game::Component(owner)
	{}
};

}
