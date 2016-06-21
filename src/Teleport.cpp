#include "Teleport.hpp"
#include "Game.hpp"
#include "GameCache.hpp"
#include "Drawable.hpp"
#include "Collider.hpp"
#include "AxisMoving.hpp"

using Game::Teleport;
using Game::TILE_SIZE;

const sf::Time Teleport::COOLDOWN_TIME = sf::milliseconds(1000);

Teleport::Teleport(const sf::Vector2f& pos) 
	: Game::Entity(pos)
{
	animated = addComponent(new Game::Animated(this, Game::getAsset("graphics", "teleport.png")));
	disableClock = addComponent(new Game::Clock(this));
	addComponent(new Game::Drawable(this, animated));
	addComponent(new Game::Collider(this, [this] (Game::Collider *c) { warp(c); }, Game::Layers::TELEPORTS));

	auto& anim = animated->addAnimation("teleport");
	// Teleports have 8 frames
	for (unsigned short i = 0; i < 8; ++i)
		anim.addFrame(sf::IntRect(i * TILE_SIZE, 0, TILE_SIZE, TILE_SIZE));
	
	auto& animatedSprite = animated->getSprite();
	animatedSprite.setAnimation(anim);
	animatedSprite.setLooped(true);
	animatedSprite.setFrameTime(sf::seconds(0.15));
	animatedSprite.play();
}

void Teleport::update() {
	Game::Entity::update();
	if (disabled && disableClock->getElapsedTime() >= COOLDOWN_TIME) {
		disabled = false;
		animated->getSprite().play();
	}
}

void Teleport::disable() {
	disabled = true;
	animated->getSprite().pause();
	disableClock->restart();
}

#include <iostream>
void Teleport::warp(Game::Collider *cld) {
	std::cerr << "Called warp"<<std::endl;
	if (disabled) return;
	
	const auto entity = cld->getOwner();
	auto am = entity->get<Game::AxisMoving>();
	std::cerr << "pos = " << cld->getOwner()->getPosition()<<" (aligned: " << entity->isAligned()<<"); cur_tile = " << Game::tile(cld->getOwner()->getPosition()) << ", prev = " << am->getPrevAlign() << " [this tile = " << Game::tile(position) << "]"<<std::endl;
	if (am != nullptr && 
			// entity must have moved here since latest warp FIXME
			(//am->getPrevAlign() == Game::tile(position) ||
			// entity must be in the same tile as this teleport, not only be touching it
			!(entity->isAligned() && Game::tile(entity->getPosition()) == Game::tile(position))))
	{
		return;
	}

	Teleport *nxt = _next;
	for ( ; nxt != nullptr && nxt != this; nxt = nxt->next()) {
		if (nxt->isDisabled() || nxt->get<Game::Collider>()->getColliding() != nullptr)
			continue;
		break;
	}

	// TODO spawn flashes
	cld->getOwnerRW()->setPosition(nxt->getPosition());
	if (am != nullptr) {
		std::cerr<<"nxt->pos = " << nxt->getPosition()<<" (tile = " <<Game::tile(nxt->getPosition())<<"\n";
		am->setPrevAlign(Game::tile(nxt->getPosition()));}

	disable();
	nxt->disable();

/*
	Teleport *next = next();
	if (next == nullptr) return;
	if (next->isDisabled() || isEntityTouching(next->getPosition())) {
		Teleport *self = next;
		bool viable = false;
		do {
			next = next->next();
			if (next == this || next->isDisabled() || isEntityTouching(next->getPosition()))
				next = next->next();
			else {
				viable = true;
				break;
			}
		} while (next != self);
		// Check if we've found an enabled destination
		if (!viable) continue;
	}

	_pushTemporary(new Game::Flash(teleport->getPosition()));
	_pushTemporary(new Game::Flash(next->getPosition()));

	// Teleport the entity
	entity->setPosition(next->getPosition());
	entity->prevAlign = Game::tile(next->getPosition());

	// Disable both source and destination for a while
	teleport->disable();
	next->disable();
*/
}
