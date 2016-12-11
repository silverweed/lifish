#pragma once

#include "Entity.hpp"

namespace Game {

class FreeMoving;
class Clock;
class Sprite;

class Fog : public Game::Entity {
	/** The time to wait before changing the alpha */
	const sf::Time alphaDt;

	Game::FreeMoving *moving = nullptr;
	Game::Sprite *sprite = nullptr;
	Game::Clock *clock = nullptr;

	bool fading = false;

public:
	explicit Fog(float speed = 10, sf::Time alphaDt = sf::seconds(0.01));

	void update() override;
};

}
