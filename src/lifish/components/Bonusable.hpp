#pragma once

#include <array>
#include <SFML/System/Time.hpp>
#include "conf/bonus.hpp"
#include "Component.hpp"
#include "bonus_type.hpp"

namespace lif {

class Clock;

/** A component that can get bonuses */
class Bonusable : public lif::Component {
protected:
	// bonus time; sf::Time::Zero means "no bonus"; negative means 'infinite'
	std::array<sf::Time, lif::conf::bonus::N_BONUS_TYPES> bonusTime;
	std::array<lif::Clock*, lif::conf::bonus::N_BONUS_TYPES> bonusClock;

public:
	explicit Bonusable(lif::Entity& owner);

	void giveBonus(lif::BonusType type, const sf::Time& time);
	bool hasBonus(lif::BonusType type) const;

	sf::Time getTime(lif::BonusType type) const;
	sf::Time getElapsedTime(lif::BonusType type) const;
	sf::Time getRemainingTime(lif::BonusType type) const;
	
	void reset();
	void expireTemporaryBonuses();
};

}
