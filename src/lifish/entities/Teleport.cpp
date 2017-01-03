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

using Game::Teleport;
using Game::TILE_SIZE;

Teleport::Teleport(const sf::Vector2f& pos) 
	: Game::Entity(pos)
{
	addComponent(new Game::Fixed(*this));
	animated = addComponent(new Game::Animated(*this, Game::getAsset("graphics", "teleport.png")));
	disableClock = addComponent(new Game::Clock(*this));
	addComponent(new Game::Drawable(*this, *animated));
	collider = addComponent(new Game::Collider(*this, [this] (Game::Collider& c) {
		warp(c);
	}, Game::Layers::TELEPORTS));
	addComponent(new Game::Spawning(*this, [this] (const Game::Spawning&) {
		return mustSpawnFlash;
	}, [this] () {
		mustSpawnFlash = false;
		return new Game::Flash(position);
	}));
	addComponent(new Game::Sounded(*this, { std::make_pair("warp", Game::getAsset("test", "teleport.ogg")) }));

	auto& anim = animated->addAnimation("teleport");
	for (unsigned short i = 0; i < N_ANIM_FRAMES; ++i)
		anim.addFrame(sf::IntRect(i * TILE_SIZE, 0, TILE_SIZE, TILE_SIZE));
	
	auto& animatedSprite = animated->getSprite();
	animatedSprite.setAnimation(anim);
	animatedSprite.setLooped(true);
	animatedSprite.setFrameTime(sf::seconds(0.15));
	animatedSprite.play();
}

void Teleport::update() {
	Game::Entity::update();
	if (disabled && disableClock->getElapsedTime() >= Game::Conf::Teleport::COOLDOWN_TIME
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

void Teleport::warp(Game::Collider& cld) {
	if (disabled) return;
	
	const auto& entity = cld.getOwner();
	auto am = entity.get<Game::AxisMoving>();
	if (am != nullptr && !(entity.isAligned() && Game::tile(entity.getPosition()) == Game::tile(position))) {
		return;
	}

	Teleport *nxt = _next;
	for ( ; nxt != nullptr && nxt != this; nxt = nxt->next()) {
		if (nxt->isDisabled() || nxt->get<Game::Collider>()->getColliding().size() > 0)
			continue;
		break;
	}

	if (nxt == nullptr || nxt == this) return;

	cld.getOwnerRW().setPosition(nxt->getPosition());
	if (am != nullptr) {
		am->setPrevAlign(Game::tile(nxt->getPosition()));
	}

	mustSpawnFlash = true;
	nxt->mustSpawnFlash = true;
	Game::cache.playSound(get<Game::Sounded>()->getSoundFile("warp"));
	disable();
	nxt->disable();
}
