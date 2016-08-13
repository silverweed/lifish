#include "LevelTime.hpp"

using Game::LevelTime;

LevelTime::LevelTime(sf::Time time)
	: Game::Entity()
	, initialTime(time)
{
	clock = addComponent(new Game::Clock(*this)); 
}

sf::Time LevelTime::getRemainingTime() const {
	return initialTime - clock->getElapsedTime();
}

void LevelTime::setTime(sf::Time time) {
	initialTime = time;
	reset();
	pause();
}

void LevelTime::update() {
	Game::Entity::update();
	if (isHurryUp) return;

	int diff = int(getRemainingTime().asSeconds());

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
