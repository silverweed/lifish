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
#include "conf/bonus.hpp"

using lif::Bonus;
using lif::TILE_SIZE;
using lif::conf::bonus::EXPIRE_TIME;
	
Bonus::Bonus(const sf::Vector2f& pos, const lif::BonusType type)
	: lif::Entity(pos)
	, type(type)
{
	sprite = addComponent<lif::Sprite>(*this,
				lif::getAsset("graphics", "bonuses.png"), 
				sf::IntRect(
					static_cast<unsigned short>(type) * TILE_SIZE, 
					0,
					TILE_SIZE,
					TILE_SIZE));
	addComponent<lif::Collider>(*this, [this] (const lif::Collider& cld) {
		if (cld.getLayer() != lif::c_layers::PLAYERS || grabbable->isGrabbed())
			return;
		_grab(static_cast<lif::Player&>(cld.getOwnerRW()));
	}, lif::c_layers::GRABBABLE);
	addComponent<lif::Drawable>(*this, *sprite);
	addComponent<lif::Scored>(*this, lif::conf::bonus::VALUE);
	expireClock = addComponent<lif::Clock>(*this);
	addComponent<lif::Sounded>(*this, lif::Sounded::SoundList {
		std::make_pair("grab", lif::getAsset("test", "bonus_grab.ogg")) 
	});
	addComponent<lif::Temporary>(*this, [this] () {
		// expire condition
		return grabbable->isGrabbed() || expireClock->getElapsedTime() > EXPIRE_TIME;
	});
	grabbable = addComponent<lif::Grabbable>(*this);
}

void Bonus::update() {
	lif::Entity::update();
	const float s = expireClock->getElapsedTime().asSeconds();
	if (EXPIRE_TIME.asSeconds() - s <= 3.) {
		const float diff = s - std::floor(s);
		if (5 * diff - std::floor(5 * diff) < 0.5)
			sprite->getSprite().setColor(sf::Color(0, 0, 0, 0));
		else
			sprite->getSprite().setColor(sf::Color(255, 255, 255, 255));
	}
}

void Bonus::_grab(lif::Player& player) {
	get<lif::Scored>()->setTarget(player.getInfo().id);
	grabbable->setGrabbingEntity(&player);
}
