#pragma once

#include "Entity.hpp"

namespace Game {

/**
 * A generic component of a game entity. Inherit this to add behavior.
 */
class Component : public Game::Entity {
protected:
	Game::Entity *const owner;

public:
	explicit Component(Game::Entity *const owner) 
		: owner(owner) {}

	/** Gets the owner of this component */
	const Game::Entity* getOwner() const { return owner; }

	/** Gets the owner of this component (non-const) */
	Game::Entity* getOwnerRW() const { return owner; }
};

}
