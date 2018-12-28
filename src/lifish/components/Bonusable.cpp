#include "Bonusable.hpp"
#include "Time.hpp"
#include "core.hpp"

using lif::Bonusable;

Bonusable::Bonusable(lif::Entity& owner)
	: lif::Component(owner)
{
	_declComponent<Bonusable>();
	bonusTime.fill(sf::Time::Zero);
	bonusT.fill(sf::Time::Zero);
}

void Bonusable::update() {
	lif::Component::update();

	const auto delta = lif::time.getDelta();
	for (auto& t : bonusT)
		t += delta;
}

void Bonusable::giveBonus(lif::BonusType type, const sf::Time& time) {
	const auto i = static_cast<std::size_t>(type);
	bonusTime[i] = time;
	bonusT[i] = sf::Time::Zero;
}

bool Bonusable::hasBonus(lif::BonusType type) const {
	const auto i = static_cast<std::size_t>(type);
	return bonusTime[i] < sf::Time::Zero ||
		(bonusTime[i] > sf::Time::Zero && bonusT[i] <= bonusTime[i]);
}

sf::Time Bonusable::getTime(lif::BonusType type) const {
	return bonusTime[static_cast<std::size_t>(type)];
}

sf::Time Bonusable::getElapsedTime(lif::BonusType type) const {
	return bonusT[static_cast<std::size_t>(type)];
}

sf::Time Bonusable::getRemainingTime(lif::BonusType type) const {
	const auto i = static_cast<std::size_t>(type);
	return std::max(sf::Time::Zero, bonusTime[i] - bonusT[i]);
}

void Bonusable::reset() {
	bonusTime.fill(sf::Time::Zero);
}

void Bonusable::expireTemporaryBonuses() {
	for (unsigned i = 0; i < bonusTime.size(); ++i) {
		if (bonusTime[i] > sf::Time::Zero)
			bonusTime[i] = sf::Time::Zero;
	}
}
