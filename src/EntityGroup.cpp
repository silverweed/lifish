#include "EntityGroup.hpp"
#include "Component.hpp"
#include "Killable.hpp"
#include <algorithm>

using Game::EntityGroup;

EntityGroup::~EntityGroup() {
	// Don't destroy unowned entities
	for (auto& e : entities) {
		if (unowned.find(e.get()) != unowned.end())
			e.release();
	}
}

void EntityGroup::setOrigin(const sf::Vector2f& origin) {
	WithOrigin::setOrigin(origin);

	for (auto& e : entities)
		e->setOrigin(origin);
}

void EntityGroup::updateAll() {
	for (auto& e : entities)
		e->update();

	_removeExpiredTemporaries();
	if (dying.size() > 0)
		_removeDying();
}

void EntityGroup::remove(Game::Entity *entity) {
	const auto cld = entity->get<Game::Collider>();
	const auto tmp = entity->get<Game::Temporary>();
	const auto klb = entity->get<Game::Killable>();
	entities.remove_if([entity] (const std::unique_ptr<Game::Entity>& e) { return e.get() == entity; });
	for (auto it = unowned.begin(); it != unowned.end(); ) {
		if (*it == entity) it = unowned.erase(it);
		else ++it;
	}
	if (cld != nullptr)
		collidingEntities.erase(std::remove(collidingEntities.begin(), collidingEntities.end(), cld));
	if (tmp != nullptr)
		temporary.remove(tmp);
	if (klb != nullptr)
		dying.remove(klb);
}

void EntityGroup::_removeExpiredTemporaries() {
	for (auto it = temporary.begin(); it != temporary.end(); ) {
		auto tmp = *it;
		if (tmp->isExpired()) {
			// If object is Killable, call its onKill()
			auto k = tmp->getOwner()->get<Game::Killable>();
			if (k != nullptr) {
				k->kill();
				if (k->isKillInProgress()) {
					// Will be finalized later
					dying.push_back(k);
					it = temporary.erase(it);
					continue;
				}
			}

			auto eit = std::find_if(entities.begin(), entities.end(), 
					[tmp] (const std::unique_ptr<Game::Entity>& ptr) 
			{
				return ptr.get() == tmp->getOwner();
			});
			if (eit != entities.end())
				entities.erase(eit);
			
			it = temporary.erase(it);
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
			if (eit != entities.end()) 
				entities.erase(eit);
			
			it = dying.erase(it);
		} else
			++it;
	}
}
