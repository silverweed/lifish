#pragma once

#include <array>
#include <memory>
#include "Boss.hpp"
#include "game.hpp"
#include "state_function.hpp"

namespace lif {

class Animated;
class Clock;
class ShootingPattern;

class HauntingSpiritBoss : public lif::Boss {

	bool selectedNewPattern = false;
	std::vector<std::weak_ptr<lif::Killable>> statues;
	std::weak_ptr<lif::Killable> targetStatue;
	/**
	 * 0. circle
	 * 1. spiral
	 * 2. scatter
	 */
	std::array<lif::ShootingPattern*, 3> shootPatterns;
	std::array<sf::Color, 3> shootColors;

	lif::ShootingPattern *curShootPattern = nullptr;
	lif::Animated *animated = nullptr;
	lif::Clock *animClock = nullptr,
	           *atkClock = nullptr,   // used for shooting
	           *hauntClock = nullptr; // used for changing haunted statue after delay

	lif::ai::StateFunction stateFunction = std::bind(&HauntingSpiritBoss::_updateStart, this);

	lif::ai::StateFunction _updateStart();
	lif::ai::StateFunction _updateSearching();
	lif::ai::StateFunction _updateSelectNewStatue();
	lif::ai::StateFunction _updateTransitioningBegin();
	lif::ai::StateFunction _updateTransitioningEnd();
	lif::ai::StateFunction _updateHaunting();
	lif::ai::StateFunction _updateDying();

	bool _isShooting() const;
public:
	explicit HauntingSpiritBoss(const sf::Vector2f& pos);

	void update() override;
};

}
