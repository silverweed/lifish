#pragma once

#include <SFML/Graphics.hpp>
#include "EntityGroup.hpp"

namespace Debug {

class DebugRenderer {
public:
	static void drawColliders(sf::RenderTarget& target, const Game::EntityGroup& group);
};

}
