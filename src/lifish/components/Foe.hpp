#pragma once

#include "Component.hpp"

namespace Game {

/**
 * This is a tagging Component with no functionality.
 * The current level is considered "clear" when there are no "Foe" Entities left.
 */
class Foe : public Game::Component {
public:
	explicit Foe(Game::Entity& owner) : Game::Component(owner) {}
};

}
