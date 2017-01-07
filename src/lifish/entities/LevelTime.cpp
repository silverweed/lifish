#include "LevelTime.hpp"
#include "conf/global.hpp"
#include "Clock.hpp"

using lif::LevelTime;

LevelTime::LevelTime(sf::Time time)
	: lif::Entity()
	, initialTime(time)
{
	clock = addComponent(new lif::Clock(*this)); 
	extraGameClock = addComponent(new lif::Clock(*this)); 
}

sf::Time LevelTime::getRemainingTime() const {
	return initialTime - clock->getElapsedTime();
}

sf::Time LevelTime::getRemainingExtraGameTime() const {
	return lif::conf::EXTRA_GAME_DURATION - extraGameClock->getElapsedTime();
}

void LevelTime::startExtraGame() {
	extraGameClock->restart();
}

void LevelTime::setTime(sf::Time time) {
	initialTime = time;
	reset();
	pause();
}

void LevelTime::update() {
	lif::Entity::update();
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
	extraGameClock->restart();
}

void LevelTime::pause() {
	clock->pause();
	extraGameClock->pause();
}

void LevelTime::resume() {
	clock->resume();
	extraGameClock->resume();
}
