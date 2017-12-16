#pragma once

#include <SFML/Graphics.hpp>

namespace lif {

class EntityGroup;
class SHCollisionDetector;

namespace debug {

class DebugRenderer final {
public:
	DebugRenderer() = delete;

	static void drawColliders(const lif::EntityGroup& group);
	static void drawSHCells(const lif::SHCollisionDetector& group);
};

}

}
