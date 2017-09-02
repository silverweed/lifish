#pragma once

#include "Entity.hpp"

namespace lif {

class Level;

class Torch : public lif::Entity {
public:
	explicit Torch(const sf::Vector2f& pos, unsigned tileRadius = 2);

	/** Visually tethers the torch to a wall if possible by flipping it in the
	 *  right direction.
	 */
	void fixOrientation(const lif::Level& level);
	void fixOrientation(const std::string& tilemap, int lvWidth);
};

}
