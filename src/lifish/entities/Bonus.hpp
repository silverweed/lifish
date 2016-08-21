#pragma once

#include <SFML/System.hpp>
#include "Sprite.hpp"
#include "BonusType.hpp"
#include "Clock.hpp"
#include "Player.hpp"
#include "Entity.hpp"
#include "Grabbable.hpp"

namespace Game {

/**
 * The bonuses dropped by walls
 */
class Bonus : public Game::Entity {
	const Game::BonusType type;

	Game::Clock *expireClock = nullptr;
	Game::Sprite *sprite = nullptr;
	Game::Grabbable *grabbable = nullptr;

	void _grab(Game::Player& player);
public:
	explicit Bonus(const sf::Vector2f& pos, const Game::BonusType type);

	Game::BonusType getType() const { return type; }

	void update() override;
};

}
