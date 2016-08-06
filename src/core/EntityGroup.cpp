#include "EntityGroup.hpp"
#include "Component.hpp"
#include "Killable.hpp"
#include <algorithm>

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
	
void EntityGroup::_removeFromInternal(const Game::Entity& entity) {
	const auto tile = Game::tile(entity.getPosition());
	if (getFixedAt(tile.x, tile.y) == &entity)
		_setFixedAt(tile.x, tile.y, nullptr);
}

void EntityGroup::remove(const Game::Entity& entity) {
	_removeFromInternal(entity);
	entities.remove_if([entity] (const std::shared_ptr<Game::Entity>& e) { return e.get() == &entity; });
}

Game::Entity* EntityGroup::_putInAux(Game::Entity *entity) {
	entity->setOrigin(origin);

	// Put in aux collections
	auto klb = entity->getShared<Game::Killable>();
	if (klb != nullptr) {
		killables.push_back(klb);
	} 

	auto cld = entity->getShared<Game::Collider>();
	if (cld != nullptr && !cld->isPhantom())
		collidingEntities.push_back(cld);

	// Put an entity marked as `Fixed` in fixedEntities
	if (entity->get<Game::Fixed>() != nullptr) {
		const auto tile = Game::tile(entity->getPosition());
		if (getFixedAt(tile.x, tile.y) != nullptr)
			throw std::logic_error("Two fixed entities share the same tile?!");

		_setFixedAt(tile.x, tile.y, entity);
	}

	return entity;
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
		if (it->expired()) {
			it = killables.erase(it);
			continue;
		}
		auto klb = it->lock();
		if (klb->isKilled()) {
			// Remove this from fixedEntities
			const auto tile = Game::tile(klb->getOwner().getPosition());
			if (getFixedAt(tile.x, tile.y) == &klb->getOwner())
				_setFixedAt(tile.x, tile.y, nullptr);

			if (klb->isKillInProgress()) {
				// Will be finalized later
				dying.push_back(klb);
				it = killables.erase(it);
				continue;
			}

			auto eit = std::find_if(entities.begin(), entities.end(), 
					[klb] (const std::shared_ptr<Game::Entity>& ptr) 
			{
				return ptr.get() == &klb->getOwner();
			});
			if (eit != entities.end()) {
				entities.erase(eit);
			}	

			it = killables.erase(it);
		} else 
			++it;
	}
}

void EntityGroup::_removeDying() {
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
				// Remove from internal collections, save for `killables`, from which it was
				// already removed by _removeExpiredTemporaries
				const auto tile = Game::tile(eit->get()->getPosition());
				if (getFixedAt(tile.x, tile.y) == eit->get())
					_setFixedAt(tile.x, tile.y, nullptr);

				entities.erase(eit);
			}
			
			it = dying.erase(it);
		} else
			++it;
	}
}

bool EntityGroup::isFixed(const Game::Entity& entity) const {
	const auto tile = Game::tile(entity.getPosition());
	const auto fixed = getFixedAt(tile.x, tile.y);
	return fixed == &entity;
}
