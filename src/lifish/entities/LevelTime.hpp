#pragma once

#include "Component.hpp"
#include "Clock.hpp"

namespace Game {

class LevelTime : public Game::Entity {
public:
	enum class HurryUpResponse {
		HURRY_UP_ON,
		HURRY_UP_NEAR,
		HURRY_UP_OFF
	};

private:
	Game::Clock *clock = nullptr;
	sf::Time initialTime;
	bool isHurryUp = false;
	bool hurryUpWarningGiven = false;
	HurryUpResponse hurryUpResponse = HurryUpResponse::HURRY_UP_OFF;

public:
	explicit LevelTime(sf::Time time = sf::Time::Zero);

	/** Sets the initialTime to `time`, resets and pauses the clock */
	void setTime(sf::Time time);
	/** Gets the remaining time */
	sf::Time getRemainingTime() const;
	HurryUpResponse checkHurryUp() const;
	void pause();
	void resume();
	void reset();

	void update() override;
};

}
