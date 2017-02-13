#include "EntityGroup.hpp"
#include "Component.hpp"
#include "Killable.hpp"
#include <algorithm>
#include <sstream>
#include <iostream>

using lif::EntityGroup;

EntityGroup::EntityGroup() {}

void EntityGroup::validate() {
	_pruneAll();
	alreadyPrunedThisUpdate = true;
}

void EntityGroup::updateAll() {
	_checkDead();
	_checkKilled();

	if (!alreadyPrunedThisUpdate)
		_pruneAll();

	for (auto& e : entities)
		e->update();

	alreadyPrunedThisUpdate = false;
}

void EntityGroup::remove(const lif::Entity& entity) {
	mtxLock();
	std::remove_if(entities.begin(), entities.end(), [entity] (std::shared_ptr<lif::Entity> e) {
		return e.get() == &entity;
	});
	_pruneAll();
	mtxUnlock();
}

void EntityGroup::remove(std::shared_ptr<lif::Entity> entity) {
	mtxLock();
	std::remove(entities.begin(), entities.end(), entity);
	_pruneAll();
	mtxUnlock();
}

auto EntityGroup::getEntitiesAtTile(const sf::Vector2i& tile) const -> std::vector<std::weak_ptr<lif::Entity>> {
	std::vector<std::weak_ptr<lif::Entity>> ents;
	for (auto& e : entities) {
		if (lif::tile(e->getPosition()) == tile)
			ents.push_back(e);
	}
	return ents;
}

void EntityGroup::clear() {
	mtxLock();
	entities.clear();
	mtxUnlock();
	collidingEntities.clear();
}

lif::Entity* EntityGroup::_putInAux(std::shared_ptr<lif::Entity> entity) {
	// Put in aux collections, if not already managed
	auto klb = entity->getShared<lif::Killable>();
	if (klb != nullptr) {
		killables.push_back(klb);
	} 

	for (auto cld : entity->getAllShared<lif::Collider>()) {
		if (cld != nullptr && !cld->isPhantom()) {
			collidingEntities.push_back(cld);
		}
	}

	return entity.get();
}

void EntityGroup::_pruneAll() {
	_pruneColliding();
}

void EntityGroup::_pruneColliding() {
	for (auto it = collidingEntities.begin(); it != collidingEntities.end(); ) {
		if (it->expired())
			it = collidingEntities.erase(it);
		else
			++it;
	}
}

void EntityGroup::_checkKilled() {
	for (auto it = killables.begin(); it != killables.end(); ) {
		if (it->expired()) {
			it = killables.erase(it);
			continue;
		}
		auto klb = it->lock();
		if (klb->isKilled()) {
			if (klb->isKillInProgress()) {
				// Will be finalized later
				dying.push_back(klb);
				it = killables.erase(it);
				continue;
			}

			auto eit = std::find_if(entities.begin(), entities.end(), 
					[klb] (std::shared_ptr<lif::Entity> ptr) 
			{
				return ptr.get() == &klb->getOwner();
			});
			if (eit != entities.end()) {
				mtxLock();
				entities.erase(eit);
				mtxUnlock();
			}

			it = killables.erase(it);
		} else {
			++it;
		}
	}
}

void EntityGroup::_checkDead() {
	for (auto it = dying.begin(); it != dying.end(); ) {
		if (it->expired()) {
			it = dying.erase(it);
			continue;
		}
		auto tmp = it->lock();
		if (!tmp->isKillInProgress()) {
			// kill function has ended, we can safely destroy this.
			auto eit = std::find_if(entities.begin(), entities.end(), 
					[tmp] (std::shared_ptr<lif::Entity>& ptr) 
			{
				return ptr.get() == &tmp->getOwner();
			});
			if (eit != entities.end()) {
				mtxLock();
				entities.erase(eit);
				mtxUnlock();
			}
			
			it = dying.erase(it);
		} else {
			++it;
		}
	}
}

bool EntityGroup::_isManagedKillable(std::shared_ptr<lif::Entity> entity) const {
	return std::find_if(killables.begin(), killables.end(), 
		[&entity] (std::weak_ptr<lif::Killable> p) {
			return !p.expired() && &p.lock().get()->getOwner() == entity.get();
		}) != killables.end();
}

bool EntityGroup::_isManagedCollider(std::shared_ptr<lif::Entity> entity) const {
	return std::find_if(collidingEntities.begin(), collidingEntities.end(), 
		[&entity] (std::weak_ptr<lif::Collider> p) {
			return !p.expired() && &p.lock().get()->getOwner() == entity.get();
		}) != collidingEntities.end();
}
