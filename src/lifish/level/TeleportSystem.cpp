#include "TeleportSystem.hpp"
#include "Animated.hpp"
#include "AxisMoving.hpp"
#include "Collider.hpp"
#include "Teleport.hpp"
#include "Time.hpp"
#include "Warpable.hpp"
#include "conf/teleport.hpp"
#include "game.hpp"
#include "utils.hpp"
#include <algorithm>

using lif::TeleportSystem;
using lif::conf::teleport::COOLDOWN_TIME;

static bool isMostlyAligned(const lif::Entity& entity) {
	const auto& epos = entity.getPosition();
	return lif::manhattanDistance(epos, lif::aligned2(epos)) < 4;
}

void TeleportSystem::add(lif::Teleport* tp) {
	teleports.emplace_back(tp);
	inactiveTime.emplace_back(sf::Time::Zero);
	justTeleportedTo.emplace_back(std::weak_ptr<lif::Collider>{});
}

void TeleportSystem::update() {

	const auto delta = lif::time.getDelta();

	for (unsigned i = 0; i < teleports.size(); ++i) {
		auto& inactiveT = inactiveTime[i];
		// Don't update the j.t.t. in the very first frame after we set it, or we reset it before the
		// entity actually teleported!
		if (inactiveT != COOLDOWN_TIME)
			_updateJustTeleportedTo(i);

		bool wasInactive = inactiveT > sf::Time::Zero;
		inactiveT = std::max(sf::Time::Zero, inactiveT - delta);
		if (inactiveT == sf::Time::Zero) {
			_updateActive(i);
			if (wasInactive)
				teleports[i]->animated->getSprite().play();
		}
	}

	//std::cout << "------------------------------------\n";
	//for (unsigned i = 0; i < teleports.size(); ++i)
		//std::cout << i << ": cd: " << inactiveTime[i].asSeconds() << ", jtt: " <<
			//(justTeleportedTo[i].expired()
				//? "-"
				//: lif::to_string(long(justTeleportedTo[i].lock().get()))) << "\n";
}

void TeleportSystem::_updateActive(unsigned tpIdx) {

	auto tp = teleports[tpIdx];
	// Check if there are any entities colliding with this teleport
	auto collider = tp->get<lif::Collider>();
	auto colliding = collider->getColliding();
	std::weak_ptr<lif::Collider> collided;
	for (auto& cld : colliding) {
		if (!cld.expired() && cld.lock() != justTeleportedTo[tpIdx].lock()) {
			collided = cld;
			break;
		}
	}

	auto shCollided = collided.lock();
	if (!shCollided)
		return;

	auto& entity = shCollided->getOwnerRW();

	// Check that the entity is close enough to the teleport's center
	if (!(isMostlyAligned(entity) && lif::tile2(entity.getPosition()) == lif::tile2(tp->getPosition())))
		return;

	// Ensure a next viable teleport exists
	const auto warpToIdx = _findNextViable(tpIdx, shCollided->getLayer());

	if (warpToIdx < 0)
		return;

	// Warp the entity
	if (auto mov = entity.get<lif::Moving>())
		mov->resetDistTravelledThisFrame();

	if (auto warpable = entity.get<lif::Warpable>())
		warpable->setJustWarped(true);

	auto warpedTp = teleports[warpToIdx];
	const auto warpedPos = warpedTp->getPosition();
	entity.setPosition(warpedPos);
	if (auto am = entity.get<lif::AxisMoving>()) {
		am->setPrevAlign(lif::tile(warpedPos));
		if (shCollided->getLayer() == lif::c_layers::PLAYERS)
			am->stop();
	}

	// Disable both starting and ending teleport
	tp->triggerWarpFx();
	tp->animated->getSprite().stop();
	warpedTp->triggerWarpFx();
	warpedTp->animated->getSprite().stop();
	inactiveTime[tpIdx] = COOLDOWN_TIME;
	inactiveTime[warpToIdx] = COOLDOWN_TIME;
	if (shCollided->getLayer() == lif::c_layers::PLAYERS)
		justTeleportedTo[warpToIdx] = collided;
}

int TeleportSystem::_findNextViable(unsigned startingTp, lif::c_layers::Layer layer) const {
	const auto numTp = teleports.size();
	// A 'good' next teleport must be active and not colliding with any entity which
	// is solid to the one we're about to warp. Moreover, it must not be fully occupied
	// by an entity (e.g. a player standing on it)
	const auto good = [this, layer] (unsigned id) {
		if (inactiveTime[id] > sf::Time::Zero) return false;
		const auto& cld = teleports[id]->collider->getColliding();
		return !std::any_of(cld.begin(), cld.end(), [layer] (const auto& wc) {
			if (auto c = wc.lock()) return c->getLayer() == layer || c->getOwner().isAligned();
			return false;
		});
	};

	bool found = false;
	int id = (startingTp + 1) % numTp;
	while (id != static_cast<signed>(startingTp)) {
		if (good(id)) {
			found = true;
			break;
		}
		id = (id + 1) % numTp;
	}

	return found ? id : -1;
}

void TeleportSystem::_updateJustTeleportedTo(unsigned id) {
	auto& jtt = justTeleportedTo[id];
	if (jtt.expired())
		return;

	for (auto cld : teleports[id]->collider->getColliding()) {
		if (auto c = cld.lock()) {
			if (c == jtt.lock() && isMostlyAligned(c->getOwner())) {
				return;
			}
		}
	}

	// If the saved entity does not collide with the teleport anymore, reset it.
	jtt.reset();
}

void TeleportSystem::clear() {
	teleports.clear();
	inactiveTime.clear();
	justTeleportedTo.clear();
}
