#pragma once
/**
 * A coin
 */

#include "FixedEntity.hpp"

namespace Game {

class Coin : public Game::FixedEntity {
public:
	Coin(const sf::Vector2f& pos, const std::string& texture_name);
};

}
