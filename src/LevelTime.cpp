#include "LevelTime.hpp"

using Game::LevelTime;

LevelTime::LevelTime(Game::Entity *const owner, unsigned int time)
	: Game::Component(owner)
	, initialTime(time)
{
	clock = addComponent(new Game::Clock<1>(this)); 
}

float LevelTime::getTime() const {
	float time = clock->getClock()->getElapsedTime().asSeconds();
	return initialTime - time;
}

void LevelTime::update() {
	if (isHurryUp) return;

	int diff = int(getTime());

	if (diff <= 0) {
		isHurryUp = true;
		hurryUpResponse = HurryUpResponse::HURRY_UP_ON;
	} else if (diff <= 30 && !hurryUpWarningGiven) {
		hurryUpWarningGiven = true;
		hurryUpResponse = HurryUpResponse::HURRY_UP_NEAR;
	}
}

LevelTime::HurryUpResponse LevelTime::checkHurryUp() const {
	return hurryUpResponse;
}

void LevelTime::reset() {
	isHurryUp = false;
	hurryUpWarningGiven = false;
	hurryUpResponse = HurryUpResponse::HURRY_UP_OFF;
	clock->restart();
}

void LevelTime::pause() {
	clock->pause();
}

void LevelTime::resume() {
	clock->resume();
}
