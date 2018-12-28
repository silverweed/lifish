#pragma once

#include "Entity.hpp"

namespace lif {

/** This class handles the level's time and the extra game's time */
class LevelTime : public lif::Entity {
public:
	enum class HurryUpResponse {
		HURRY_UP_ON,
		HURRY_UP_NEAR,
		HURRY_UP_OFF
	};

private:
	sf::Time initialTime;
	sf::Time remainingTime;
	sf::Time extraGameRemainingTime = sf::Time::Zero;

	bool isHurryUp = false;
	bool hurryUpWarningGiven = false;
	HurryUpResponse hurryUpResponse = HurryUpResponse::HURRY_UP_OFF;

public:
	explicit LevelTime(sf::Time time = sf::Time::Zero);

	/** Sets the initialTime to `time`, resets and pauses the clock */
	void setTime(sf::Time time);
	/** Gets the remaining level time */
	sf::Time getRemainingTime() const { return remainingTime; }
	HurryUpResponse checkHurryUp() const;

	void reset();

	/** Resets the extraGameClock */
	void startExtraGame();
	sf::Time getRemainingExtraGameTime() const { return extraGameRemainingTime; }

	void update() override;
};

}
