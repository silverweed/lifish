#include "EntityGroup.hpp"
#include "Component.hpp"
#include "Killable.hpp"
#include <algorithm>
#include <iostream>
#include <sstream>

using lif::EntityGroup;

EntityGroup::EntityGroup() {}

void EntityGroup::validate() {
	_pruneAll();
	alreadyPrunedThisUpdate = true;
}

void EntityGroup::checkAll() {
	_checkDead();
	_checkKilled();
	alreadyCheckedThisUpdate = true;
}

void EntityGroup::updateAll() {
	if (!alreadyCheckedThisUpdate)
		checkAll();

	if (!alreadyPrunedThisUpdate)
		_pruneAll();

	for (auto& e : entities)
		e->update();

	alreadyPrunedThisUpdate = false;
	alreadyCheckedThisUpdate = false;
}

auto EntityGroup::getCollidersIntersecting(const sf::FloatRect& rect) const -> std::vector<lif::Collider*> {
	std::vector<lif::Collider*> clds;
	clds.reserve(collidingEntities.size());
	for (auto& e : collidingEntities) {
		if (e.expired()) continue;
		auto cld = e.lock();
		if (cld->getRect().intersects(rect))
			clds.emplace_back(cld.get());
	}
	return clds;
}

void EntityGroup::clear() {
	entities.clear();
	collidingEntities.clear();
}

lif::Entity* EntityGroup::add(lif::Entity *entity) {
	entity->init();
	entities.emplace_back(entity);
	return _putInAux(entities.back().get());
}

lif::Entity* EntityGroup::_putInAux(lif::Entity *entity) {
	// Put in aux collections, if not already managed
	auto klb = entity->getShared<lif::Killable>();
	if (klb != nullptr) {
		killables.emplace_back(klb);
	}

	for (auto cld : entity->getAllShared<lif::Collider>()) {
		if (cld != nullptr && !cld->isPhantom()) {
			collidingEntities.emplace_back(cld);
		}
	}

	return entity;
}

void EntityGroup::_pruneAll() {
	_pruneColliding();
}

void EntityGroup::_pruneColliding() {
	collidingEntities.erase(std::remove_if(collidingEntities.begin(), collidingEntities.end(), [] (auto it) {
		return it.expired();
	}), collidingEntities.end());
}

void EntityGroup::_checkKilled() {
	// see https://www.quora.com/What-is-the-best-way-to-iterate-through-a-vector-while-popping-certain-elements-out-in-C++
	auto w = killables.begin();
	for (auto it = w; it != killables.end(); ++it) {
		if (it->expired()) {
			// erase
			continue;
		}
		auto klb = it->lock();
		if (klb->isKilled()) {
			if (klb->isKillInProgress()) {
				// Will be finalized later
				dying.emplace_back(klb);
				// erase
				continue;
			}

			auto eit = std::find_if(entities.begin(), entities.end(),
				[klb] (const std::shared_ptr<lif::Entity>& ptr)
			{
				return ptr.get() == &klb->getOwner();
			});
			if (eit != entities.end()) {
				entities.erase(eit);
			}
			// erase

		} else {
			if (it != w)
				*w = std::move(*it);
			++w;
		}
	}
	killables.erase(w, killables.end());
}

void EntityGroup::_checkDead() {
	auto w = dying.begin();
	for (auto it = w; it != dying.end(); ++it) {
		if (it->expired())
			continue;

		auto tmp = it->lock();
		if (!tmp->isKillInProgress()) {
			// kill function has ended, we can safely destroy this.
			auto eit = std::find_if(entities.begin(), entities.end(), [tmp] (const auto& ptr) {
				return ptr.get() == &tmp->getOwner();
			});
			if (eit != entities.end()) {
				entities.erase(eit);
			}

		} else {
			if (it != w)
				*w = std::move(*it);
			++w;
		}
	}
	dying.erase(w, dying.end());
}

