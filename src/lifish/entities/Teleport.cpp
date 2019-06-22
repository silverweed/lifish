#include "Teleport.hpp"
#include "Animated.hpp"
#include "AxisMoving.hpp"
#include "Collider.hpp"
#include "Drawable.hpp"
#include "Fixed.hpp"
#include "GameCache.hpp"
#include "OneShotFX.hpp"
#include "Sounded.hpp"
#include "Spawning.hpp"
#include "Time.hpp"
#include "ZIndexed.hpp"
#include "conf/teleport.hpp"
#include "conf/zindex.hpp"
#include "game.hpp"

using lif::Teleport;
using lif::TILE_SIZE;

Teleport::Teleport(const sf::Vector2f& pos)
	: lif::Entity(pos)
{
	addComponent<lif::Fixed>(*this);
	animated = addComponent<lif::Animated>(*this, lif::getAsset("graphics", "teleport.png"));
	addComponent<lif::Drawable>(*this, *animated);
	collider = addComponent<lif::Collider>(*this, [this] (lif::Collider& c) {
		_warp(c);
	}, lif::c_layers::TELEPORTS);
	addComponent<lif::ZIndexed>(*this, lif::conf::zindex::TELEPORTS);
	addComponent<lif::Spawning>(*this, [this] (const lif::Spawning&) {
		return mustSpawnFlash;
	}, [this] () {
		mustSpawnFlash = false;
		return new lif::OneShotFX(position, "flash.png", {
			sf::IntRect(0, 0, lif::TILE_SIZE, lif::TILE_SIZE),
			sf::IntRect(lif::TILE_SIZE, 0, lif::TILE_SIZE, lif::TILE_SIZE),
			sf::IntRect(2 * lif::TILE_SIZE, 0, lif::TILE_SIZE, lif::TILE_SIZE),
			sf::IntRect(3 * lif::TILE_SIZE, 0, lif::TILE_SIZE, lif::TILE_SIZE),
			sf::IntRect(2 * lif::TILE_SIZE, 0, lif::TILE_SIZE, lif::TILE_SIZE),
			sf::IntRect(lif::TILE_SIZE, 0, lif::TILE_SIZE, lif::TILE_SIZE),
			sf::IntRect(0, 0, lif::TILE_SIZE, lif::TILE_SIZE)
		});
	});
	addComponent<lif::Sounded>(*this,
		lif::sid("warp"), lif::getAsset("sounds", "teleport.ogg")
	);

	auto& anim = animated->addAnimation("teleport");
	for (unsigned i = 0; i < N_ANIM_FRAMES; ++i)
		anim.addFrame(sf::IntRect(i * TILE_SIZE, 0, TILE_SIZE, TILE_SIZE));

	auto& animatedSprite = animated->getSprite();
	animatedSprite.setAnimation(anim);
	animatedSprite.setLooped(true);
	animatedSprite.setFrameTime(sf::seconds(0.07));
	animatedSprite.play();
}

void Teleport::update() {
	lif::Entity::update();
	disableT += lif::time.getDelta();
	if (disabled &&
		disableT >= lif::conf::teleport::COOLDOWN_TIME)
	{
		disabled = false;
		animated->getSprite().play();

		for (auto& cld : collider->getColliding()) {
			if (auto c = cld.lock()) {
				_warp(*c);
				break;
			}
		}
	}
}

void Teleport::disable() {
	disabled = true;
	animated->getSprite().pause();
	disableT = sf::Time::Zero;
}

void Teleport::_warp(lif::Collider& cld) {
	if (disabled) return;

	const auto& entity = cld.getOwner();
	auto am = entity.get<lif::AxisMoving>();
	if (am != nullptr && !(entity.isAligned() && lif::tile(entity.getPosition()) == lif::tile(position))) {
		return;
	}

	// Find the first viable teleport to warp to.
	Teleport *nxt = _next;
	const auto layer = cld.getLayer();
	for ( ; nxt != nullptr && nxt != this; nxt = nxt->next()) {
		if (nxt->isDisabled())
			continue;

		bool isGood = true;
 		for (const auto& oth :  nxt->get<lif::Collider>()->getColliding()) {
			const auto other = oth.lock();
			if (other && layer == other->getLayer()) {
				isGood = false;
				break;
			}
		}

		if (isGood)
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
