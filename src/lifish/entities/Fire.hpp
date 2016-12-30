#pragma once

#include <SFML/System.hpp>
#include "Pond.hpp"

namespace Game {

/**
 * Fire is a Pond which only damages enemies
 */
class Fire : public Game::Pond {
public:
	/** Duration == sf::Time::Zero means 'infinite' */
	explicit Fire(const sf::Vector2f& pos, const sf::Vector2f& size, sf::Time duration = sf::Time::Zero);
};

}
