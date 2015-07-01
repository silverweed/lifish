#pragma once
/**
 * A coin
 */

#include <SFML/Graphics.hpp>
#include "FixedEntity.hpp"

namespace Game {

class Coin : public Game::FixedEntity {
public:
	Coin(sf::Vector2f pos, const std::string& texture_name);

	void draw(sf::RenderTarget& window) override;
};

}
