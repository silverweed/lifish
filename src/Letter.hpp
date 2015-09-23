#pragma once

#include "Animated.hpp"
#include "Scored.hpp"
#include "Sounded.hpp"

namespace Game {

/** An EXTRA letter. */
class Letter 
	: public Game::Animated
	, public Game::Scored
	, public Game::Sounded 
{
	constexpr static unsigned short TRANSITION_DELAY = 3000; // ms

	/** What letter is this (currently) */
	unsigned short id;

	/** The clock driving the letter's transitions */
	sftools::Chronometer transitionClock;

	/** Whether this letter is transitioning or not */
	bool transitioning = false;

public:
	static unsigned short randomId();

	Letter(const sf::Vector2f& pos, unsigned short id);

	unsigned short getId() const { return id; }

	/** Checks if the letter should make the transition and, if so, makes it. */
	void checkTransition();
};

}
