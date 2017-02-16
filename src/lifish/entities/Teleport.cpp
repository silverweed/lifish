#include "Teleport.hpp"
#include "Animated.hpp"
#include "Clock.hpp"
#include "game.hpp"
#include "GameCache.hpp"
#include "Flash.hpp"
#include "Drawable.hpp"
#include "Collider.hpp"
#include "AxisMoving.hpp"
#include "Fixed.hpp"
#include "Spawning.hpp"
#include "Sounded.hpp"
#include "conf/teleport.hpp"

using lif::Teleport;
using lif::TILE_SIZE;

Teleport::Teleport(const sf::Vector2f& pos) 
	: lif::Entity(pos)
{
	addComponent(std::make_shared<lif::Fixed>(*this));
	animated = addComponent(std::make_shared<lif::Animated>(*this, lif::getAsset("graphics", "teleport.png")));
	disableClock = addComponent(std::make_shared<lif::Clock>(*this));
	addComponent(std::make_shared<lif::Drawable>(*this, *animated));
	collider = addComponent(std::make_shared<lif::Collider>(*this, [this] (lif::Collider& c) {
		warp(c);
	}, lif::c_layers::TELEPORTS));
	addComponent(std::make_shared<lif::Spawning>(*this, [this] (const lif::Spawning&) {
		return mustSpawnFlash;
	}, [this] () {
		mustSpawnFlash = false;
		return new lif::Flash(position);
	}));
	addComponent(std::make_shared<lif::Sounded>(*this, lif::Sounded::SoundList {
		std::make_pair("warp", lif::getAsset("test", "teleport.ogg")) 
	}));

	auto& anim = animated->addAnimation("teleport");
	for (unsigned i = 0; i < N_ANIM_FRAMES; ++i)
		anim.addFrame(sf::IntRect(i * TILE_SIZE, 0, TILE_SIZE, TILE_SIZE));
	
	auto& animatedSprite = animated->getSprite();
	animatedSprite.setAnimation(anim);
	animatedSprite.setLooped(true);
	animatedSprite.setFrameTime(sf::seconds(0.15));
	animatedSprite.play();
}

void Teleport::update() {
	lif::Entity::update();
	if (disabled && disableClock->getElapsedTime() >= lif::conf::teleport::COOLDOWN_TIME
			&& collider->getColliding().size() == 0)
	{
		disabled = false;
		animated->getSprite().play();
	}
}

void Teleport::disable() {
	disabled = true;
	animated->getSprite().pause();
	disableClock->restart();
}

void Teleport::warp(lif::Collider& cld) {
	if (disabled) return;
	
	const auto& entity = cld.getOwner();
	auto am = entity.get<lif::AxisMoving>();
	if (am != nullptr && !(entity.isAligned() && lif::tile(entity.getPosition()) == lif::tile(position))) {
		return;
	}

	Teleport *nxt = _next;
	for ( ; nxt != nullptr && nxt != this; nxt = nxt->next()) {
		if (nxt->isDisabled() || nxt->get<lif::Collider>()->getColliding().size() > 0)
			continue;
		break;
	}

	if (nxt == nullptr || nxt == this) return;

	cld.getOwnerRW().setPosition(nxt->getPosition());
	if (am != nullptr) {
		am->setPrevAlign(lif::tile(nxt->getPosition()));
	}

	mustSpawnFlash = true;
	nxt->mustSpawnFlash = true;
	lif::cache.playSound(get<lif::Sounded>()->getSoundFile("warp"));
	disable();
	nxt->disable();
}
