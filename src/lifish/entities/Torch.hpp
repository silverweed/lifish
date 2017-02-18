#pragma once

#include "Entity.hpp"

namespace lif {

class Torch : public lif::Entity {
public:
	explicit Torch(const sf::Vector2f& pos, unsigned tileRadius = 2);
};

}
