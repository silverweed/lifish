#include "SHCollisionDetector.hpp"
#include "AxisMoving.hpp"
#include "Direction.hpp"
#include "EntityGroup.hpp"
#include "collision_utils.hpp"
#include <iostream>

using namespace lif::collision_utils;
using lif::SHContainer;
using lif::SHCollisionDetector;

////// SHContainer ///////
SHContainer::SHContainer(const sf::Vector2f& levelSize, unsigned subdivisions)
	: levelSize(levelSize)
	, cellSize(levelSize.x / subdivisions, levelSize.y / subdivisions)
	, subdivisions(subdivisions)
	, buckets(subdivisions * subdivisions)
{}

void SHContainer::clear() {
	for (auto& b : buckets)
		b.clear();
	all.clear();
}

void SHContainer::insert(std::weak_ptr<lif::Collider> obj) {
	if (obj.expired()) return;

	const auto cld = obj.lock();
	if (!cld->isActive()) return;

	const auto ids = _getIdFor(*cld);
	for (auto id : ids) {
		buckets[id].emplace_back(obj);
	}
	all.emplace_back(obj);
}

std::vector<unsigned> SHContainer::_getIdFor(const lif::Collider& obj) const {
	std::vector<unsigned> ids;

	const auto pos = obj.getPosition();
	const auto size = obj.getSize();

	const sf::Vector2i upleft(
				(pos.x - lif::TILE_SIZE) / cellSize.x,
				(pos.y - lif::TILE_SIZE) / cellSize.y),
	                   downright(
				(pos.x - lif::TILE_SIZE + size.x) / cellSize.x,
				(pos.y - lif::TILE_SIZE + size.y) / cellSize.y);

	// Insert the object in all the buckets within its vertices
	for (int i = upleft.x; i <= downright.x; ++i)
		for (int j = upleft.y; j <= downright.y; ++j)
			ids.emplace_back(j * subdivisions + i);

	ids.erase(std::remove_if(ids.begin(), ids.end(), [size = buckets.size()] (auto i) {
		return i >= size;
	}), ids.end());

#ifndef RELEASE
	// Check all elements are unique
	std::sort(ids.begin(), ids.end());
	assert(std::adjacent_find(ids.begin(), ids.end()) == ids.end());
#endif

	return ids;
}

auto SHContainer::getNearby(const lif::Collider& obj) const -> std::vector<std::weak_ptr<lif::Collider>> {
	std::vector<std::weak_ptr<lif::Collider>> nearby;
	nearby.reserve(64);

	const auto ids = _getIdFor(obj);
	for (auto id : ids) {
		//std::cerr << id << ": " << buckets[id].size() << std::endl;
		for (auto& cld : buckets[id]) {
			// Don't bother to remove invalid objects here, as this list gets regenerated
			// at every frame.
			if (cld.expired()) continue;

			auto oth = cld.lock().get();
			if (oth != &obj)
				nearby.emplace_back(cld);
		}
	}
	return nearby;
}

////// SHCollisionDetector ///////
SHCollisionDetector::SHCollisionDetector(lif::EntityGroup& group,
		const sf::FloatRect& limit, unsigned subdivisions)
	: lif::CollisionDetector(group, limit)
	, container(sf::Vector2f(limit.width - limit.left, limit.height - limit.top), subdivisions)
{
#ifndef RELEASE
	dbgStats.counter.set("subdivisions", subdivisions);
#endif
}

void SHCollisionDetector::setLevelLimit(const sf::FloatRect& limit) {
	lif::CollisionDetector::setLevelLimit(limit);
	container.levelSize = sf::Vector2f(limit.width - limit.left, limit.height - limit.top);
	container.cellSize = sf::Vector2f(container.levelSize.x / container.subdivisions,
	                                  container.levelSize.y / container.subdivisions);
}

void SHCollisionDetector::update() {
#ifndef RELEASE
	// Container setup time
	dbgStats.timer.start("setup");
#endif
	container.clear();

	/* For each moving entity, check (towards its direction):
	 * 1) has it reached the level boundaries?
	 * 2) is there another non-trasparent entity occupying the cell ahead?
	 */
	const auto& colliding = group.getColliding();
	for (auto it = colliding.begin(); it != colliding.end(); ++it) {
		// No need to check for expired, as EntityGroup prunes them before we're called
		auto collider = it->lock();
		// reset collider
		collider->reset();
		collider->setAtLimit(false);
		container.insert(*it);
	}

#ifndef RELEASE
	dbgStats.timer.end("setup");
	// Total time taken
	dbgStats.timer.start("tot");
	// Time taken by all narrow checks
	dbgStats.timer.set("tot_narrow", 0);
	// Number of narrow-checked entities
	dbgStats.counter.reset("checked");
#endif

	// Collision detection loop
	const auto& all = container.getAll();
	for (auto it = all.begin(); it != all.end(); ++it) {
		auto collider = it->lock();

		// Fixed entities only collide passively
		if (collider->getOwner().get<lif::Fixed>() != nullptr)
			continue;

		const auto moving = collider->getOwner().get<lif::Moving>();
		const auto axismoving = moving ? dynamic_cast<lif::AxisMoving*>(moving) : nullptr;
		if (moving && isAtBoundaries(*collider, axismoving, levelLimit)) {
			collider->setAtLimit(true);
			continue;
		}

		for (auto oth : container.getNearby(*collider)) {
			if (oth.expired()) continue;
#ifndef RELEASE
			dbgStats.counter.inc("checked");
			dbgStats.timer.start("single");
#endif
			auto othcollider = oth.lock().get();

			if (axismoving) {
				// Only check entities ahead of this one
				if (!directionIsViable(*collider, *axismoving, *othcollider))
					continue;

				if (collider->collidesWith(*othcollider)
						&& collide(*collider, *othcollider, axismoving->getDirection()))
				{
					//std::cerr << &collider->getOwner() << " colliding with " << &othcollider->getOwner()<<std::endl;
					collider->addColliding(oth);
					if (collider->requestsForceAck() || othcollider->requestsForceAck()
							|| othcollider->getOwner().get<lif::Moving>() == nullptr)
					{
						// Let the entity know we collided with it.
						// We only do that for non-moving entities to avoid problems with
						// multiple collisions between two moving entities.
						othcollider->addColliding(*it);
					}
				}
			} else if (collider->contains(*othcollider) && collider->collidesWith(*othcollider)) {
				collider->addColliding(oth);
				othcollider->addColliding(*it);
			}

#ifndef RELEASE
			dbgStats.timer.set("tot_narrow", dbgStats.timer.get("tot_narrow")
					+ dbgStats.timer.end("single"));
#endif
		}
	}

#ifndef RELEASE
	dbgStats.timer.end("tot");
#endif
}
