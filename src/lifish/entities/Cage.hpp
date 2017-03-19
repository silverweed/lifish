#pragma once

#include "Entity.hpp"

namespace lif {

class Cage : public lif::Entity {
public:
	explicit Cage(const sf::Vector2f& pos);
};

}
