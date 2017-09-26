#pragma once

#include <array>
#include <memory>
#include "Boss.hpp"
#include "game.hpp"

namespace lif {

class Animated;
class Clock;
class ShootingPattern;

class HauntingSpiritBoss : public lif::Boss {
	enum class State {
		START,
		SEARCHING,
		SELECT_NEW_STATUE,
		TRANSITIONING_BEGIN,
		TRANSITIONING_END,
		HAUNTING,
		DYING
	} state;

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

	void _updateStart();
	void _updateSearching();
	void _updateSelectNewStatue();
	void _updateTransitioningBegin();
	void _updateTransitioningEnd();
	void _updateHaunting();
	void _updateDying();
	bool _isShooting() const;
public:
	explicit HauntingSpiritBoss(const sf::Vector2f& pos);

	void update() override;
};

}
