#pragma once

#include "Component.hpp"
#include "Clock.hpp"

namespace Game {

class LevelTime : public Game::Component {
public:
	enum class HurryUpResponse {
		HURRY_UP_ON,
		HURRY_UP_NEAR,
		HURRY_UP_OFF
	};

private:
	Game::Clock<1> *clock;
	const unsigned int initialTime;
	bool isHurryUp = false;
	bool hurryUpWarningGiven = false;
	HurryUpResponse hurryUpResponse = HurryUpResponse::HURRY_UP_OFF;

public:
	explicit LevelTime(Game::Entity *const owner, unsigned int time);

	float getTime() const;
	void update() override;
	HurryUpResponse checkHurryUp() const;
	void pause();
	void resume();
	void reset();
};

}
