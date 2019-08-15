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
#include "utils.hpp"

using lif::Teleport;
using lif::TILE_SIZE;

Teleport::Teleport(const sf::Vector2f& pos)
	: lif::Entity(pos)
{
	//addComponent<lif::Fixed>(*this);
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
		if (collider->getColliding().size() == 0) {
			disabled = false;
			animated->getSprite().play();
		} else if (auto c = scheduledWarpCld.lock()) {
			disabled = false;
			_warp(*c);
			scheduledWarpCld.reset();
		}
	}
}

void Teleport::disable() {
	disabled = true;
	animated->getSprite().pause();
	disableT = sf::Time::Zero;
}

void Teleport::_warp(lif::Collider& cld) {
	if (disabled) {
		if (!scheduledWarpCld.lock()) {
			const auto am = cld.getOwner().get<lif::AxisMoving>();
			if (am != nullptr && am->getDistTravelledThisFrame() > 1) {
				bool ok = true;
				// UGLY HACK:
				// check if the potential scheduled collider is being "pushed" by someone.
				// If that is the case, the distTravelledThisFrame is spurious, so pretend the
				// guy is still. This is an edge case, so we may want to do something different
				// eventually.
				if (cld.getLayer() == lif::c_layers::PLAYERS) {
					for (auto c : cld.getColliding()) {
						auto other = c.lock();
						if (other && other->isSolidFor(cld) &&
								other->getOwner().get<lif::AxisMoving>() != nullptr)
						{
							ok = false;
							break;
						}
					}
				}
				if (ok)
					scheduledWarpCld = cld.getOwner().getShared<lif::Collider>();
			}
		}
		return;
	}

	const auto& entity = cld.getOwner();
	auto am = entity.get<lif::AxisMoving>();

	if (am != nullptr) {
		const auto& epos = entity.getPosition();
		const bool mostlyAligned = lif::manhattanDistance(epos, lif::aligned2(epos)) < 4;
		if (!(mostlyAligned && lif::tile2(entity.getPosition()) == lif::tile2(position)))
			return;
	}

	// Find the first viable teleport to warp to.
	Teleport *nxt = _next;
	const auto layer = cld.getLayer();
	for ( ; nxt != nullptr && nxt != this; nxt = nxt->next()) {
		if (nxt->isDisabled())
			continue;

		bool isGood = true;
 		for (const auto& oth : nxt->collider->getColliding()) {
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

	if (auto mov = cld.getOwner().get<lif::Moving>()) {
		mov->resetDistTravelledThisFrame();
	}
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
