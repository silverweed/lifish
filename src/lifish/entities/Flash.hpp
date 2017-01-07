#pragma once

#include <SFML/System.hpp>
#include "Entity.hpp"

namespace lif {

class Animated;

/**
 * The flash made by a Teleport
 */
class Flash : public lif::Entity {
	lif::Animated *animated = nullptr;

public:
	explicit Flash(const sf::Vector2f& pos);
};

}
