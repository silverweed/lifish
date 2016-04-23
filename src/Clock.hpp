#pragma once

#include <initializer_list>
#include "Chronometer.hpp"
#include "Component.hpp"

namespace Game {

/**
 * An object with one or more pausable clocks
 */
template <unsigned short NClocks = 1>
class Clock : public Game::Component {
	std::array<sftools::Chronometer, NClocks> clocks;
	std::array<std::string, NClocks> clockNames;


	sftools::Chronometer* _getClock(const std::string& name) const {
		auto it = clockNames.find(name);
		if (it == clockNames.end()) return nullptr;
		return &(*it);
	}

public:
	explicit Clock(Game::Entity *const owner)
		: Game::Component(owner) {}

	explicit Clock(Game::Entity *const owner,
			std::initializer_list<std::string> _clockNames)
		: Game::Component(owner)
	{
		static_assert(_clockNames.size() == NClocks, "Invalid length of clockNames initializer_list!");
		unsigned short i = 0;
		for (auto name : _clockNames)
			clockNames[i++] = name;
	}

	sftools::Chronometer* getClock(const std::string& name) {
		return _getClock(name);
	}

	sftools::Chronometer* getClock(unsigned short i) {
		if (i < clocks.size())
			return &clocks[i];
		return nullptr;
	}

	void pause() {
		for (auto& clock : clocks)
			clock.pause();
	}

	void pause(const std::string& name) {
		_getClock(name)->pause();
	}

	void resume() {
		for (auto& clock : clocks)
			clock.resume();
	}

	void resume(const std::string& name) {
		_getClock(name)->resume();
	}

	void restart() {
		for (auto& clock : clocks)
			clock.restart(true);
	}

	sf::Time restart(const std::string& name) {
		return _getClock(name)->restart();
	}

	sf::Time getElapsedTime(const std::string& name) const {
		return _getClock(name)->getElapsedTime();
	}
};

template <>
class Clock<1> : public Game::Component {
	sftools::Chronometer clock;

public:
	explicit Clock(Game::Entity *const owner)
		: Game::Component(owner)
	{}

	sftools::Chronometer* getClock() {
		return &clock;
	}

	void pause(const std::string& name = "") {
		clock.pause();
	}

	void resume(const std::string& name = "") {
		clock.resume();
	}

	sf::Time restart(const std::string& name = "") {
		return clock.restart();
	}

	sf::Time getElapsedTime() const {
		return clock.getElapsedTime();
	}
};

}
