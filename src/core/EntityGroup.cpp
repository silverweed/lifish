#include "EntityGroup.hpp"
#include "Component.hpp"
#include "Killable.hpp"
#include <algorithm>
#include <sstream>
#include <iostream>

using Game::EntityGroup;

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

void EntityGroup::remove(const Game::Entity& entity) {
	mtxLock();
	entities.remove_if([entity] (std::shared_ptr<Game::Entity> e) { return e.get() == &entity; });
	_pruneAll();
	mtxUnlock();
}

void EntityGroup::remove(std::shared_ptr<Game::Entity> entity) {
	mtxLock();
	entities.remove(entity);
	_pruneAll();
	mtxUnlock();
}

auto EntityGroup::getEntitiesAtTile(const sf::Vector2i& tile) const -> std::vector<std::weak_ptr<Game::Entity>> {
	std::vector<std::weak_ptr<Game::Entity>> ents;
	for (auto& e : entities) {
		if (Game::tile(e->getPosition()) == tile)
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

Game::Entity* EntityGroup::_putInAux(std::shared_ptr<Game::Entity> entity) {
	// Put in aux collections, if not already managed
	auto klb = entity->getShared<Game::Killable>();
	if (klb != nullptr) {
		killables.push_back(klb);
	} 

	for (auto cld : entity->getAllShared<Game::Collider>()) {
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
					[klb] (std::shared_ptr<Game::Entity> ptr) 
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
					[tmp] (std::shared_ptr<Game::Entity>& ptr) 
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

bool EntityGroup::_isManagedKillable(std::shared_ptr<Game::Entity> entity) const {
	return std::find_if(killables.begin(), killables.end(), 
		[&entity] (std::weak_ptr<Game::Killable> p) {
			return !p.expired() && &p.lock().get()->getOwner() == entity.get();
		}) != killables.end();
}

bool EntityGroup::_isManagedCollider(std::shared_ptr<Game::Entity> entity) const {
	return std::find_if(collidingEntities.begin(), collidingEntities.end(), 
		[&entity] (std::weak_ptr<Game::Collider> p) {
			return !p.expired() && &p.lock().get()->getOwner() == entity.get();
		}) != collidingEntities.end();
}
