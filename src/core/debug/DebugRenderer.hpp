#pragma once

#include <SFML/Graphics.hpp>

namespace Game {

class EntityGroup;
class SHCollisionDetector;

namespace Debug {

class DebugRenderer {
public:
	static void drawColliders(sf::RenderTarget& target, const Game::EntityGroup& group);
	static void drawSHCells(sf::RenderTarget& target, const Game::SHCollisionDetector& group);
};

}

}
