#include "LevelTime.hpp"
#include "Time.hpp"
#include "conf/global.hpp"
#include "core.hpp"

using lif::LevelTime;

LevelTime::LevelTime(sf::Time time)
	: lif::Entity()
	, initialTime(time)
{
	reset();
}

void LevelTime::startExtraGame() {
	extraGameRemainingTime = lif::conf::EXTRA_GAME_DURATION;
}

void LevelTime::setTime(sf::Time time) {
	initialTime = time;
	reset();
}

void LevelTime::update() {
	lif::Entity::update();

	const auto delta = lif::time.getDelta();
	remainingTime -= delta;
	if (extraGameRemainingTime > sf::Time::Zero)
		extraGameRemainingTime -= delta;
}

LevelTime::HurryUpResponse LevelTime::checkHurryUp() const {
	return remainingTime > sf::seconds(30) ? HurryUpResponse::HURRY_UP_OFF
		: remainingTime > sf::Time::Zero ? HurryUpResponse::HURRY_UP_NEAR
		: HurryUpResponse::HURRY_UP_ON;
}

void LevelTime::reset() {
	remainingTime = initialTime;
	extraGameRemainingTime = sf::Time::Zero;
}
