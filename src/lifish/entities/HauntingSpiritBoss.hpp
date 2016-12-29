#pragma once

#include <array>
#include <memory>
#include "Boss.hpp"
#include "game.hpp"

namespace Game {

class HauntedStatue;
class Animated;
class Clock;
class ShootingPoint;
class ShootingPattern;

class HauntingSpiritBoss : public Game::Boss {
	enum class State {
		START,
		SEARCHING,
		TRANSITIONING_BEGIN,
		TRANSITIONING_END,
		HAUNTING,
		DYING
	} state;

	std::vector<std::weak_ptr<Game::HauntedStatue>> statues;
	std::weak_ptr<Game::HauntedStatue> targetStatue;
	std::array<Game::ShootingPattern*, 1> shootPatterns;

	Game::Clock *animClock = nullptr,
	            *hauntClock = nullptr;

	void _updateStart();
	void _updateSearching();
	void _updateTransitioningBegin();
	void _updateTransitioningEnd();
	void _updateHaunting();
	void _updateDying();
public:
	explicit HauntingSpiritBoss(const sf::Vector2f& pos);

	void update() override;
};

}
