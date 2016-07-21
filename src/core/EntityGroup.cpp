#include "EntityGroup.hpp"
#include "Component.hpp"
#include "Killable.hpp"
#include <algorithm>

#include <iostream>

using Game::EntityGroup;

EntityGroup::EntityGroup() {
	fixedEntities.fill(nullptr);
}

void EntityGroup::setOrigin(const sf::Vector2f& origin) {
	WithOrigin::setOrigin(origin);

	for (auto& e : entities)
		e->setOrigin(origin);
}

void EntityGroup::updateAll() {
	_checkKilled();
	if (dying.size() > 0)
		_removeDying();

	for (auto& e : entities)
		e->update();
}
	
void EntityGroup::_removeFromColliding(const Game::Entity *const entity) {
	const auto cld = entity->get<Game::Collider>();
	if (cld != nullptr)
		collidingEntities.erase(std::remove(collidingEntities.begin(), collidingEntities.end(), cld));
}

void EntityGroup::_removeFromFixed(const Game::Entity *const entity) {
	const auto tile = Game::tile(entity->getPosition());
	if (getFixedAt(tile.x, tile.y) == entity)
		_setFixedAt(tile.x, tile.y, nullptr);
}

void EntityGroup::_removeFromKillables(const Game::Entity *const entity, bool removeFromDying) {
	const auto klb = entity->get<Game::Killable>();
	if (klb != nullptr) {
		killables.remove(klb);
		if (removeFromDying)
			dying.remove(klb);
	}
}

void EntityGroup::_removeFromInternal(const Game::Entity *const entity) {
	if (entity == nullptr) return;
	_removeFromColliding(entity);
	_removeFromFixed(entity);
	_removeFromKillables(entity, true);
}

void EntityGroup::remove(Game::Entity *entity) {
	_removeFromInternal(entity);
	entities.remove_if([entity] (const std::unique_ptr<Game::Entity>& e) { return e.get() == entity; });
}

Game::Entity* EntityGroup::release(Game::Entity *e) {
	Game::Entity *released = nullptr;
	for (auto it = entities.begin(); it != entities.end(); ) {
		if (it->get() == e) {
			released = it->release();
			_removeFromInternal(it->get());
			it = entities.erase(it);
			break;
		} else 
			++it;
	}
	return released;
}

void EntityGroup::clear() {
	entities.clear();
	collidingEntities.clear();
	killables.clear();
	dying.clear();
	fixedEntities.fill(nullptr);
}

void EntityGroup::_checkKilled() {
	for (auto it = killables.begin(); it != killables.end(); ) {
		auto klb = *it;
		if (klb->isKilled()) {
			klb->kill();
			const auto entity = klb->getOwner();
			if (klb->isKillInProgress()) {
				// Will be finalized later
				_removeFromFixed(entity);
				dying.push_back(klb);
				it = killables.erase(it);
				continue;
			}
			
			auto eit = std::find_if(entities.begin(), entities.end(), 
					[klb] (const std::unique_ptr<Game::Entity>& ptr) 
			{
				return ptr.get() == klb->getOwner();
			});
			if (eit != entities.end()) {
				_removeFromFixed(eit->get());
				_removeFromColliding(eit->get());
				entities.erase(eit);
			}
			
			it = killables.erase(it);
		} else 
			++it;
	}
}

void EntityGroup::_removeDying() {
	for (auto it = dying.begin(); it != dying.end(); ) {
		auto tmp = *it;
		if (!tmp->isKillInProgress()) {
			// kill function has ended, we can safely destroy this.
			auto eit = std::find_if(entities.begin(), entities.end(), 
					[tmp] (std::unique_ptr<Game::Entity>& ptr) 
			{
				return ptr.get() == tmp->getOwner();
			});
			if (eit != entities.end()) {
				// Remove from internal collections, save for `killables`, from which it was
				// already removed by _removeExpiredTemporaries
				_removeFromFixed(eit->get());
				_removeFromColliding(eit->get());

				entities.erase(eit);
			}
			
			it = dying.erase(it);
		} else
			++it;
	}
}
