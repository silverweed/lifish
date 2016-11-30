#include "EntityGroup.hpp"
#include "Component.hpp"
#include "Killable.hpp"
#include <algorithm>
#include <sstream>
#include <iostream>

using Game::EntityGroup;

EntityGroup::EntityGroup() {
	for (auto& f : fixedEntities)
		f.clear();
}

void EntityGroup::setOrigin(const sf::Vector2f& origin) {
	WithOrigin::setOrigin(origin);

	for (auto& e : entities)
		e->setOrigin(origin);
}

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
	mtxUnlock();
}

void EntityGroup::refresh(const Game::Entity& entity) {
	// FIXME
	/*auto eit = std::find_if(entities.begin(), entities.end(), [&entity] (std::shared_ptr<Game::Entity> e) {
		return e.get() == &entity;
	});
	if (eit == entities.end()) 
		throw std::invalid_argument("entity passed to EntityGroup::refresh() is not in this group!");
	*/

	std::remove_if(killables.begin(), killables.end(), [&entity] (std::weak_ptr<Game::Killable> k) {
		return k.expired() || &k.lock().get()->getOwner() == &entity;
	});
	auto klb = entity.getShared<Game::Killable>();
	if (klb != nullptr) {
		killables.push_back(klb);
	} 

	std::remove_if(collidingEntities.begin(), collidingEntities.end(),
		[&entity] (std::weak_ptr<Game::Collider> c) 
	{
		return c.expired() || &c.lock().get()->getOwner() == &entity;
	});
	auto cld = entity.getShared<Game::Collider>();
	if (cld != nullptr && !cld->isPhantom()) {
		collidingEntities.push_back(cld);
	}

	// FIXME
	/*
	const auto tile = Game::tile(entity.getPosition());
	auto& fxd = fixedEntities[(tile.y - 1) * Game::LEVEL_WIDTH + tile.x - 1];
	std::remove_if(fxd.begin(), fxd.end(), [&entity] (std::weak_ptr<Game::Entity> e) {
		return e.expired() || e.lock().get() == &entity;
	});
	if (entity.get<Game::Fixed>() != nullptr) {
		_addFixedAt(tile.x, tile.y, *eit);
	}
	*/
}


void EntityGroup::clear() {
	mtxLock();
	entities.clear();
	mtxUnlock();
	collidingEntities.clear();
}

std::vector<std::reference_wrapper<Game::Entity>> EntityGroup::getFixedAt(unsigned short x, unsigned short y) const {
	std::vector<std::reference_wrapper<Game::Entity>> fxd;

	if (x < 1 || x > Game::LEVEL_WIDTH || y < 1 || y > Game::LEVEL_HEIGHT)
		return fxd;

	for (const auto& f : _fixedAt(sf::Vector2i(x, y))) 
		if (!f.expired()) {
			auto shd = f.lock();
			if (shd.get() != nullptr)
				fxd.push_back(*shd.get());
		}
	return fxd;
}

auto EntityGroup::_fixedAt(const sf::Vector2i& tile) -> std::vector<std::weak_ptr<Game::Entity>>& {
	return fixedEntities[(tile.y - 1) * Game::LEVEL_WIDTH + tile.x - 1];
}

auto EntityGroup::_fixedAt(const sf::Vector2i& tile) const -> const std::vector<std::weak_ptr<Game::Entity>>& {
	return fixedEntities[(tile.y - 1) * Game::LEVEL_WIDTH + tile.x - 1];
}

void EntityGroup::_pruneAll() {
	_pruneFixed();
	_pruneColliding();
}

void EntityGroup::_pruneFixed() {
	for (auto& f : fixedEntities) {
		for (auto it = f.begin(); it != f.end(); ) {
			if (it->expired())
				it = f.erase(it);
			else
				++it;
		}
	}
}

void EntityGroup::_pruneColliding() {
	for (auto it = collidingEntities.begin(); it != collidingEntities.end(); ) {
		if (it->expired())
			it = collidingEntities.erase(it);
		else
			++it;
	}
}

Game::Entity* EntityGroup::_putInAux(std::shared_ptr<Game::Entity> entity) {
	entity->setOrigin(origin);

	// Put in aux collections, if not already managed
	auto klb = entity->getShared<Game::Killable>();
	if (klb != nullptr && !_isManagedKillable(entity)) {
		killables.push_back(klb);
	} 

	for (auto cld : entity->getAllRecursiveShared<Game::Collider>()) {
		if (cld != nullptr && !cld->isPhantom() && !_isManagedCollider(entity)) {
			collidingEntities.push_back(cld);
		}
	}

	// Put an entity marked as `Fixed` in fixedEntities
	if (entity->get<Game::Fixed>() != nullptr) {
		const auto tile = Game::tile(entity->getPosition());
		if (!_isManagedFixed(entity))
			_addFixedAt(tile.x, tile.y, entity);
	}

	return entity.get();
}

void EntityGroup::_addFixedAt(unsigned short x, unsigned short y, std::shared_ptr<Game::Entity> e) {
	if (x < 1 || x > Game::LEVEL_WIDTH || y < 1 || y > Game::LEVEL_HEIGHT)
		return;
	fixedEntities[(y - 1) * Game::LEVEL_WIDTH + x - 1].push_back(e);
}

void EntityGroup::_rmFixedAt(unsigned short x, unsigned short y, const Game::Entity& entity) {
	if (x < 1 || x > Game::LEVEL_WIDTH || y < 1 || y > Game::LEVEL_HEIGHT)
		return;
	auto& fixed = fixedEntities[(y - 1) * Game::LEVEL_WIDTH + x - 1];
	std::remove_if(fixed.begin(), fixed.end(), [entity] (std::weak_ptr<Game::Entity> e) {
		return !e.expired() && e.lock().get() == &entity;
	});
}

void EntityGroup::_checkKilled() {
	for (auto it = killables.begin(); it != killables.end(); ) {
		if (it->expired()) {
			it = killables.erase(it);
			continue;
		}
		auto klb = it->lock();
		if (klb->isKilled()) {
			// Remove this from fixedEntities
			const auto tile = Game::tile(klb->getOwner().getPosition());
			_rmFixedAt(tile.x, tile.y, klb->getOwner());

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

bool EntityGroup::_isManagedFixed(std::shared_ptr<Game::Entity> entity) const {
	const auto& fx = _fixedAt(Game::tile(entity.get()->getPosition()));
	return std::find_if(fx.begin(), fx.end(),
		[&entity] (std::weak_ptr<Game::Entity> p) {
			return !p.expired() && p.lock().get() == entity.get();
		}) != fx.end();
}
