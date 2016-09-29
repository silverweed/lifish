#include "Bonus.hpp"
#include "Drawable.hpp"
#include "Sounded.hpp"
#include "Sprite.hpp"
#include "Grabbable.hpp"
#include "Clock.hpp"
#include "Player.hpp"
#include "Scored.hpp"
#include "collision_layers.hpp"
#include "Temporary.hpp"
#include "Collider.hpp"
#include "game.hpp"
#include "game_values.hpp"

using Game::Bonus;
using Game::TILE_SIZE;
using Game::Conf::Bonus::EXPIRE_TIME;
	
Bonus::Bonus(const sf::Vector2f& pos, const Game::BonusType type)
	: Game::Entity(pos)
	, type(type)
{
	sprite = addComponent(new Game::Sprite(*this,
				Game::getAsset("graphics", "bonuses.png"), 
				sf::IntRect(
					static_cast<unsigned short>(type) * TILE_SIZE, 
					static_cast<unsigned short>(type) / 10 * TILE_SIZE, 
					TILE_SIZE,
					TILE_SIZE)));
	addComponent(new Game::Collider(*this, [this] (const Game::Collider& cld) {
		if (grabbable->isGrabbed()) return;
		// on collision (no check since its layer only collides with players)
		_grab(static_cast<Game::Player&>(cld.getOwnerRW()));
	}, Game::Layers::GRABBABLE));
	addComponent(new Game::Drawable(*this, *sprite));
	addComponent(new Game::Scored(*this, Game::Conf::Bonus::VALUE));
	expireClock = addComponent(new Game::Clock(*this));
	addComponent(new Game::Sounded(*this, { Game::getAsset("test", "bonus_grab.ogg") }));
	addComponent(new Game::Temporary(*this, [this] () {
		// expire condition
		return grabbable->isGrabbed() || expireClock->getElapsedTime() > EXPIRE_TIME;
	}));
	grabbable = addComponent(new Game::Grabbable(*this));
}

void Bonus::update() {
	Game::Entity::update();
	const float s = expireClock->getElapsedTime().asSeconds();
	if (EXPIRE_TIME.asSeconds() - s <= 3.) {
		const float diff = s - std::floor(s);
		if (5 * diff - std::floor(5 * diff) < 0.5)
			sprite->getSprite().setColor(sf::Color(0, 0, 0, 0));
		else
			sprite->getSprite().setColor(sf::Color(255, 255, 255, 255));
	}
}

void Bonus::_grab(Game::Player& player) {
	get<Game::Scored>()->setTarget(player.getInfo().id);
	grabbable->setGrabbingEntity(&player);
}
