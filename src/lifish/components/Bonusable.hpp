#pragma once

#include <array>
#include <SFML/System/Time.hpp>
#include "game_values.hpp"
#include "Component.hpp"
#include "bonus_type.hpp"

namespace Game {

class Clock;

/** A component that can get bonuses */
class Bonusable : public Game::Component {
protected:
	// bonus time; sf::Time::Zero means "no bonus"; negative means 'infinite'
	std::array<sf::Time, Game::Conf::Bonus::N_BONUS_TYPES> bonusTime;
	std::array<Game::Clock*, Game::Conf::Bonus::N_BONUS_TYPES> bonusClock;

public:
	explicit Bonusable(Game::Entity& owner);

	void giveBonus(Game::BonusType type, const sf::Time& time);
	bool hasBonus(Game::BonusType type) const;

	sf::Time getTime(Game::BonusType type) const;
	sf::Time getElapsedTime(Game::BonusType type) const;
	sf::Time getRemainingTime(Game::BonusType type) const;
	
	void reset();
	void expireTemporaryBonuses();
};

}
