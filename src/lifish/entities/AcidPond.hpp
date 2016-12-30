#pragma once

#include "Pond.hpp"

namespace Game {

/** A Pond which deals 1 damage to players stepping on it. */
class AcidPond : public Game::Pond {
public:
	explicit AcidPond(const sf::Vector2f& pos, const sf::Vector2f& size);
};

}
