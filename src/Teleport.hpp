#pragma once
/**
 * A teleport
 */

#include "FixedEntity.hpp"
#include "Animated.hpp"

namespace Game {

class Teleport : public Game::FixedEntity, public Game::Animated {
public:
	Teleport(const sf::Vector2f& pos);

	void draw(sf::RenderTarget& window) override;
	void setOrigin(const sf::Vector2f& origin) override { Game::Animated::setOrigin(origin); } 
};

}
