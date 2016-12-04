#include "Bonusable.hpp"
#include "Clock.hpp"

using Game::Bonusable;

Bonusable::Bonusable(Game::Entity& owner)
	: Game::Component(owner)
{
	bonusClock.fill(addComponent(new Game::Clock(*this)));
	bonusTime.fill(sf::Time::Zero);
}

void Bonusable::giveBonus(Game::BonusType type, const sf::Time& time) {
	const auto i = static_cast<unsigned short>(type);
	bonusTime[i] = time;
	bonusClock[i]->restart();
}

bool Bonusable::hasBonus(Game::BonusType type) const {
	const auto i = static_cast<unsigned short>(type);
	return bonusTime[i] < sf::Time::Zero || 
		(bonusTime[i] > sf::Time::Zero && bonusClock[i]->getElapsedTime() <= bonusTime[i]);
}

sf::Time Bonusable::getTime(Game::BonusType type) const {
	return bonusTime[static_cast<unsigned short>(type)]; 
}

sf::Time Bonusable::getElapsedTime(Game::BonusType type) const {
	return bonusClock[static_cast<unsigned short>(type)]->getElapsedTime();
}

sf::Time Bonusable::getRemainingTime(Game::BonusType type) const {
	const auto i = static_cast<unsigned short>(type);
	return std::max(sf::Time::Zero, bonusTime[i] - bonusClock[i]->getElapsedTime());
}

void Bonusable::reset() {
	bonusTime.fill(sf::Time::Zero);
}

void Bonusable::expireTemporaryBonuses() {
	for (unsigned short i = 0; i < bonusTime.size(); ++i) {
		if (bonusTime[i] > sf::Time::Zero)
			bonusTime[i] = sf::Time::Zero;
	}
}
