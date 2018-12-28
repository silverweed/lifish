#pragma once

#include "Component.hpp"
#include "bonus_type.hpp"
#include "conf/bonus.hpp"
#include <SFML/System/Time.hpp>
#include <array>

namespace lif {

/** A component that can get bonuses */
class Bonusable : public lif::Component {
protected:
	// bonus time; sf::Time::Zero means "no bonus"; negative means 'infinite'
	std::array<sf::Time, lif::conf::bonus::N_BONUS_TYPES> bonusTime;
	std::array<sf::Time, lif::conf::bonus::N_BONUS_TYPES> bonusT;

public:
	explicit Bonusable(lif::Entity& owner);

	void update() override;

	void giveBonus(lif::BonusType type, const sf::Time& time);
	bool hasBonus(lif::BonusType type) const;

	sf::Time getTime(lif::BonusType type) const;
	sf::Time getElapsedTime(lif::BonusType type) const;
	sf::Time getRemainingTime(lif::BonusType type) const;

	void reset();
	void expireTemporaryBonuses();
};

}
