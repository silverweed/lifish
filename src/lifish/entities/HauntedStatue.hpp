#pragma once

#include "Entity.hpp"

namespace Game {

class HauntedStatue : public Game::Entity {
public:
	explicit HauntedStatue(const sf::Vector2f& pos);
};

}
