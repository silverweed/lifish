#pragma once

#include <SFML/Graphics.hpp>
#include "EntityGroup.hpp"

namespace Game {

namespace Debug {

class DebugRenderer {
public:
	static void drawColliders(sf::RenderTarget& target, const Game::EntityGroup& group);
};

}

}
