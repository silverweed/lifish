#pragma once

#include <SFML/System.hpp>
#include "bonus_type.hpp"
#include "Entity.hpp"

namespace lif {

class Clock;
class Sprite;
class Grabbable;
class Player;

/**
 * The bonuses dropped by walls
 */
class Bonus : public lif::Entity {
	const lif::BonusType type;

	lif::Clock *expireClock = nullptr;
	lif::Sprite *sprite = nullptr;
	lif::Grabbable *grabbable = nullptr;

	void _grab(lif::Player& player);
public:
	explicit Bonus(const sf::Vector2f& pos, const lif::BonusType type);

	lif::BonusType getType() const { return type; }

	void update() override;
};

}
