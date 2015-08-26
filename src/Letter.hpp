#pragma once

#include "Animated.hpp"
#include "FixedEntity.hpp"
#include "Scored.hpp"
#include "Sounded.hpp"

namespace Game {

/** An EXTRA letter. */
class Letter : public Game::FixedEntity, public Game::Animated, public Game::Scored, public Game::Sounded {
	constexpr static unsigned short TRANSITION_DELAY = 3000; // ms

	/** What letter is this (currently) */
	unsigned short id;

	/** The clock driving the letter's transitions */
	sf::Clock transitionClock;

	/** Whether this letter is transitioning or not */
	bool transitioning = false;
public:
	static unsigned short randomId();

	Letter(const sf::Vector2f& pos, unsigned short id);

	void draw(sf::RenderTarget& window) override {
		Game::Animated::draw(window);
	}

	void setOrigin(const sf::Vector2f& pos) override {
		Game::Animated::setOrigin(pos);
	}

	unsigned short getId() const { return id; }

	/** Checks if the letter should make the transition and, if so, makes it. */
	void checkTransition();
};

}
