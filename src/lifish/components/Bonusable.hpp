#pragma once

#include <array>
#include "Clock.hpp"
#include "Bonus.hpp"
#include "Entity.hpp"

namespace Game {

/** A component that can get bonuses */
class Bonusable : public Game::Component {
protected:
	// bonus time; sf::Time::Zero means "no bonus"; negative means 'infinite'
	std::array<sf::Time, Game::Bonus::N_BONUS_TYPES> bonusTime;
	std::array<Game::Clock*, Game::Bonus::N_BONUS_TYPES> bonusClock;

public:
	explicit Bonusable(Game::Entity& owner)
		: Game::Component(owner)
	{
		bonusClock.fill(addComponent(new Game::Clock(*this)));
		bonusTime.fill(sf::Time::Zero);
	}

	void giveBonus(Game::Bonus::Type type, const sf::Time& time) {
		const auto i = static_cast<unsigned short>(type);
		bonusTime[i] = time;
		bonusClock[i]->restart();
	}

	bool hasBonus(Game::Bonus::Type type) const {
		const auto i = static_cast<unsigned short>(type);
		return bonusTime[i] != sf::Time::Zero && bonusClock[i]->getElapsedTime() <= bonusTime[i];
	}

	sf::Time getTime(Game::Bonus::Type type) const {
		return bonusTime[static_cast<unsigned short>(type)]; 
	}

	sf::Time getElapsedTime(Game::Bonus::Type type) const {
		return bonusClock[static_cast<unsigned short>(type)]->getElapsedTime();
	}

	sf::Time getRemainingTime(Game::Bonus::Type type) const {
		const auto i = static_cast<unsigned short>(type);
		return std::max(sf::Time::Zero, bonusTime[i] - bonusClock[i]->getElapsedTime());
	}
	
	void reset() {
		bonusTime.fill(sf::Time::Zero);
	}

};

}
