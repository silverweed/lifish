#include "HauntingSpiritBoss.hpp"
#include "HauntedStatue.hpp"
#include "Animated.hpp"
#include "Drawable.hpp"
#include "Clock.hpp"
#include "FreeSighted.hpp"
#include "core.hpp"
#include <algorithm>

#include <iostream>

using Game::HauntingSpiritBoss;

HauntingSpiritBoss::HauntingSpiritBoss(const sf::Vector2f& pos)
	: Game::Boss(pos)
	, state(State::START)
{
	// This boss has no Lifed component: it dies when there are no HauntedStatues left in the level.
	addComponent(new Game::FreeSighted(*this))->setActive(false);
	animated = addComponent(new Game::Animated(*this, Game::getAsset("graphics", "haunting_spirit.png"))); 
	const auto size = 4 * Game::TILE_SIZE;
	animated->addAnimation("idle", {
		sf::IntRect(0, 0, size, size),
		sf::IntRect(size, 0, size, size),
		sf::IntRect(2 * size, 0, size, size),
		sf::IntRect(3 * size, 0, size, size),
		sf::IntRect(4 * size, 0, size, size),
		sf::IntRect(5 * size, 0, size, size)
	}, true);
	animated->addAnimation("start", {
		sf::IntRect(0, size, size, size),
		sf::IntRect(size, size, size, size),
		sf::IntRect(2 * size, size, size, size),
		sf::IntRect(3 * size, size, size, size),
		sf::IntRect(4 * size, size, size, size),
		sf::IntRect(5 * size, size, size, size)
	});
	animated->getSprite().setOrigin(size/2, size/2);
	addComponent(new Game::Drawable(*this, *animated));
	animClock = addComponent(new Game::Clock(*this));
	hauntClock = addComponent(new Game::Clock(*this));
}

void HauntingSpiritBoss::update() {
	Game::Boss::update();

	std::cerr << "\rstate = " << int(state);
	switch (state) {
	case State::START:
		_updateStart();
		break;
	case State::SEARCHING:
		_updateSearching();
		break;
	case State::TRANSITIONING:
		_updateTransitioning();
		break;
	case State::HAUNTING:
		_updateHaunting();
		break;
	case State::DYING:
		_updateDying();
		break;
	}
}

void HauntingSpiritBoss::_updateStart() {
	// Task: play initial animation after a delay
	if (animClock->getElapsedTime() < sf::seconds(2)) return;
	if (!animated->getSprite().isPlaying()) {
		animated->getSprite().setLooped(true);	
		animated->setAnimation("idle");
		get<Game::FreeSighted>()->setActive(true);
		state = State::SEARCHING;
	} else if (animated->getAnimationName() == "idle") {
		animated->getSprite().setLooped(false, false);	
		animated->setAnimation("start");
	}
}

void HauntingSpiritBoss::_updateSearching() {
	// Task: find all the HauntedStatues in the level. Do this once and keep track of them internally thereafter.
	auto sighted = get<Game::FreeSighted>();
	const auto seen = sighted->entitiesSeen();
	for (auto& pair : seen) {
		if (auto statue = std::dynamic_pointer_cast<Game::HauntedStatue>(pair.first.lock()))
			statues.push_back(statue);
	}
	sighted->setActive(false); // no need for this anymore
	state = State::TRANSITIONING;
}

void HauntingSpiritBoss::_updateTransitioning() {
	// Task: play the transitioning animation and set the next haunted statue as `possessed`

}

void HauntingSpiritBoss::_updateHaunting() {
	// Task: attack the player; leave after some delay
}

void HauntingSpiritBoss::_updateDying() {
	// Task: play death animation
}
