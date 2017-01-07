#pragma once

#include <SFML/Graphics.hpp>

namespace lif {

class EntityGroup;
class SHCollisionDetector;

namespace debug {

class DebugRenderer {
public:
	static void drawColliders(sf::RenderTarget& target, const lif::EntityGroup& group);
	static void drawSHCells(sf::RenderTarget& target, const lif::SHCollisionDetector& group);
};

}

}
