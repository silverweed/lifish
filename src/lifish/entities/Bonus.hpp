#pragma once

#include <SFML/System.hpp>
#include "Sprite.hpp"
#include "BonusType.hpp"
#include "Clock.hpp"
#include "Player.hpp"
#include "Entity.hpp"

namespace Game {

/**
 * The bonuses dropped by walls
 */
class Bonus : public Game::Entity {
	const Game::BonusType type;

	bool grabbed = false;
	Game::Clock *expireClock = nullptr;
	Game::Sprite *sprite = nullptr;
	Game::Player *grabbingPlayer = nullptr;

	void _grab(Game::Player& player);
public:
	explicit Bonus(const sf::Vector2f& pos, const Game::BonusType type);

	Game::BonusType getType() const { return type; }

	/** `grab` shall be called when the Bonus triggers its effect to avoid reprocess it */
	void grab() { grabbed = true; }
	bool isGrabbed() const { return grabbed; }

	Game::Player* getGrabbingPlayer() const { return grabbingPlayer; }

	void update() override;
};

}
