#pragma once

#include "OneShotFX.hpp"
#include "core.hpp"

namespace lif {

class SmokeRing : public lif::OneShotFX {
public:
	explicit SmokeRing(const sf::Vector2f& pos)
		: lif::OneShotFX(pos, "smoke_ring.png", {
			sf::IntRect(0 * lif::TILE_SIZE, 0, 2 * lif::TILE_SIZE, 2 * lif::TILE_SIZE),
			sf::IntRect(2 * lif::TILE_SIZE, 0, 2 * lif::TILE_SIZE, 2 * lif::TILE_SIZE),
			sf::IntRect(4 * lif::TILE_SIZE, 0, 2 * lif::TILE_SIZE, 2 * lif::TILE_SIZE),
			sf::IntRect(6 * lif::TILE_SIZE, 0, 2 * lif::TILE_SIZE, 2 * lif::TILE_SIZE),
			sf::IntRect(8 * lif::TILE_SIZE, 0, 2 * lif::TILE_SIZE, 2 * lif::TILE_SIZE),
		})
	{}
};

}
