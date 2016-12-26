#pragma once

#include "Entity.hpp"

namespace Game {

class HauntedStatue : public Game::Entity {
	bool possessed = false;

public:
	explicit HauntedStatue(const sf::Vector2f& pos);

	void setPossessed(bool b) { possessed = b; }
	bool isPossessed() const { return possessed; }
};

}
