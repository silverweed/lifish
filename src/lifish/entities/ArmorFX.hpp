#pragma once

#include "OneShotFX.hpp"

namespace lif {

class ArmorFX : public lif::OneShotFX {
public:
	explicit ArmorFX(const sf::Vector2f& pos)
		: lif::OneShotFX(pos, "armor.png", {
			sf::IntRect(0 * lif::TILE_SIZE, 0, lif::TILE_SIZE, lif::TILE_SIZE),
			sf::IntRect(1 * lif::TILE_SIZE, 0, lif::TILE_SIZE, lif::TILE_SIZE),
			sf::IntRect(2 * lif::TILE_SIZE, 0, lif::TILE_SIZE, lif::TILE_SIZE),
			sf::IntRect(3 * lif::TILE_SIZE, 0, lif::TILE_SIZE, lif::TILE_SIZE),
			sf::IntRect(4 * lif::TILE_SIZE, 0, lif::TILE_SIZE, lif::TILE_SIZE),
			sf::IntRect(5 * lif::TILE_SIZE, 0, lif::TILE_SIZE, lif::TILE_SIZE),
		}) {
			animated->getSprite().setColor(sf::Color(255, 255, 255, 180));
		}
};

}
