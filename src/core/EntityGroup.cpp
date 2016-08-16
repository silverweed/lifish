#include "EntityGroup.hpp"
#include "Component.hpp"
#include "Killable.hpp"
#include <algorithm>
#include <sstream>

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

void EntityGroup::updateAll() {
	_pruneFixed();
	_checkKilled();
	if (dying.size() > 0)
		_removeDying();

	for (auto& e : entities)
		e->update();
}

void EntityGroup::remove(const Game::Entity& entity) {
	entities.remove_if([entity] (const std::shared_ptr<Game::Entity>& e) { return e.get() == &entity; });
}

void EntityGroup::clear() {
	entities.clear();
	collidingEntities.clear();
}

std::vector<std::reference_wrapper<Game::Entity>> EntityGroup::getFixedAt(unsigned short x, unsigned short y) const {
	std::vector<std::reference_wrapper<Game::Entity>> fxd;

	if (x < 1 || x > Game::LEVEL_WIDTH || y < 1 || y > Game::LEVEL_HEIGHT)
		return fxd;

	for (const auto& f : fixedEntities[(y - 1) * Game::LEVEL_WIDTH + x - 1]) 
		if (!f.expired()) {
			auto shd = f.lock();
			if (shd.get() != nullptr)
				fxd.push_back(*shd.get());
		}
	return fxd;
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

Game::Entity* EntityGroup::_putInAux(const std::shared_ptr<Game::Entity>& entity) {
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
		_addFixedAt(tile.x, tile.y, entity);
	}

	return entity.get();
}

void EntityGroup::_addFixedAt(unsigned short x, unsigned short y, const std::shared_ptr<Game::Entity>& e) {
	if (x < 1 || x > Game::LEVEL_WIDTH || y < 1 || y > Game::LEVEL_HEIGHT)
		return;
	fixedEntities[(y - 1) * Game::LEVEL_WIDTH + x - 1].push_back(e);
}

void EntityGroup::_rmFixedAt(unsigned short x, unsigned short y, const Game::Entity& entity) {
	if (x < 1 || x > Game::LEVEL_WIDTH || y < 1 || y > Game::LEVEL_HEIGHT)
		return;
	auto& fixed = fixedEntities[(y - 1) * Game::LEVEL_WIDTH + x - 1];
	std::remove_if(fixed.begin(), fixed.end(), [entity] (const std::weak_ptr<Game::Entity>& e) {
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
					[klb] (const std::shared_ptr<Game::Entity>& ptr) 
			{
				return ptr.get() == &klb->getOwner();
			});
			if (eit != entities.end())
				entities.erase(eit);

			it = killables.erase(it);
		} else {
			++it;
		}
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
			if (eit != entities.end())
				entities.erase(eit);
			
			it = dying.erase(it);
		} else {
			++it;
		}
	}
}
