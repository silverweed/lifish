#pragma once

#include <SFML/System.hpp>
#include "Entity.hpp"
#include "Clock.hpp"
#include "AnimatedSprite.hpp"

namespace Game {

/** An EXTRA letter. */
class Letter : public Game::Entity {
	const static sf::Time TRANSITION_DELAY;

	/** What letter is this (currently) */
	unsigned short id;

	/** The clock driving the letter's transitions */
	Game::Clock<1> *transitionClock = nullptr;
	AnimatedSprite* animatedSprite = nullptr;

	/** Whether this letter is transitioning or not */
	bool transitioning = false;

public:
	static unsigned short randomId();

	explicit Letter(const sf::Vector2f& pos, unsigned short id);

	unsigned short getId() const { return id; }

	void update() override;
};

}
