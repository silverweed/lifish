#pragma once

#include "OneShotFX.hpp"

namespace lif {

class Blink : public lif::OneShotFX {
public:
	explicit Blink(const sf::Vector2f& pos)
		: lif::OneShotFX(pos, "blink.png", {
			sf::IntRect(0 * 2 * lif::TILE_SIZE, 0, 2 * lif::TILE_SIZE, 2 * lif::TILE_SIZE),
			sf::IntRect(1 * 2 * lif::TILE_SIZE, 0, 2 * lif::TILE_SIZE, 2 * lif::TILE_SIZE),
			sf::IntRect(2 * 2 * lif::TILE_SIZE, 0, 2 * lif::TILE_SIZE, 2 * lif::TILE_SIZE),
			sf::IntRect(3 * 2 * lif::TILE_SIZE, 0, 2 * lif::TILE_SIZE, 2 * lif::TILE_SIZE),
			sf::IntRect(4 * 2 * lif::TILE_SIZE, 0, 2 * lif::TILE_SIZE, 2 * lif::TILE_SIZE),
		}) {}
};

}
