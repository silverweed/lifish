#pragma once

#include "OneShotFX.hpp"

namespace lif {

/**
 * The flash made by a Teleport
 */
class Flash : public lif::OneShotFX {
public:
	explicit Flash(const sf::Vector2f& pos)
		: lif::OneShotFX(pos, "flash.png", {
			sf::IntRect(0, 0, lif::TILE_SIZE, lif::TILE_SIZE),
			sf::IntRect(lif::TILE_SIZE, 0, lif::TILE_SIZE, lif::TILE_SIZE),
			sf::IntRect(2 * lif::TILE_SIZE, 0, lif::TILE_SIZE, lif::TILE_SIZE),
			sf::IntRect(3 * lif::TILE_SIZE, 0, lif::TILE_SIZE, lif::TILE_SIZE),
			sf::IntRect(2 * lif::TILE_SIZE, 0, lif::TILE_SIZE, lif::TILE_SIZE),
			sf::IntRect(lif::TILE_SIZE, 0, lif::TILE_SIZE, lif::TILE_SIZE),
			sf::IntRect(0, 0, lif::TILE_SIZE, lif::TILE_SIZE)
		}) {}
};

}
