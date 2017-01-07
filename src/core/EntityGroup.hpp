#pragma once

#include <memory>
#include <list>
#include <array>
#include <iterator>
#include <unordered_set>
#include <functional>
#include <SFML/System/NonCopyable.hpp>
#include "Entity.hpp"
#include "Temporary.hpp"
#include "Killable.hpp"
#include "Moving.hpp"
#include "Collider.hpp"
#include "Fixed.hpp"
#ifdef MULTITHREADED
#	include <mutex>
#endif

namespace Game {

class CollisionDetector;
class LevelRenderer;

namespace {
	template<class T>
	struct identity {
		typedef T type;
	};

	template<class T, typename... Args>
	using AppliedFunc = typename identity<std::function<void(T, Args...)>>::type;
}

/**
 * A container for Entities, providing convenient methods for operating
 * on all or a specific type of them.
 */
class EntityGroup final : private sf::NonCopyable {

	bool alreadyPrunedThisUpdate = false;

#ifdef MULTITHREADED
	mutable std::mutex mutex;
#endif

	/** All the entities (owning references) */
	std::list<std::shared_ptr<Game::Entity>> entities;

	/** The colliders of entities which have one */
	std::vector<std::weak_ptr<Game::Collider>> collidingEntities;

	/** The list of the killable entities, which ought to be removed when
	 *  their `isKilled()` method yields true.
	 */
	std::list<std::weak_ptr<Game::Killable>> killables;

	/** The list of Killable entities who are being destroyed (those whose isKilled() is
	 *  true but isKillInProgress() is true as well).
	 */
	std::list<std::weak_ptr<Game::Killable>> dying;


	/** Removes any killed entity from all internal collections (including the main one) and destroys them.
	 *  If its `isKillInProgress()` is true, puts it in `dying`
	 *  instead of immediately destroing it (it is not removed from `entities` until it's finalized)
	 */
	void _checkKilled();
	/** Removes any entity in `dying` which has `isKillInProgress() == false` and destroys them (this also
	 *  removes them from the main collections).
	 */
	void _checkDead();

	/** Iterate over aux collections and remove all expired weak pointers. 
	 *  Note that, differently from the `_check*` methods, the `_prune*` ones do NOT
	 *  affect the main `entities` collection.
	 */
	void _pruneAll();
	void _pruneColliding();

	Game::Entity* _putInAux(std::shared_ptr<Game::Entity> entity);

	/** @return whether a Killable in this group's `killables` refers to `entity` */
	bool _isManagedKillable(std::shared_ptr<Game::Entity> entity) const;
	/** @return whether a Collider in this group's `collidingEntities` refers to `entity` */
	bool _isManagedCollider(std::shared_ptr<Game::Entity> entity) const;

public:
	/**
	 * Constructs the EntityGroup as the owner of its entities.
	 */
	explicit EntityGroup();

	/** Applies a void(Args...) function to all entities (ref-args version)  */
	template<typename... Args>
	void apply(AppliedFunc<Game::Entity*, Args&...> func, Args&... args);

	/** Applies a void(Args...) function to all entities (const version) */
	template<typename... Args>
	void apply(AppliedFunc<const Game::Entity*, Args...> func, Args... args) const;

	/** Applies a void(Args...) function to all entities (weak_ptr version) */
	template<typename... Args>
	void apply(AppliedFunc<std::weak_ptr<Game::Entity>, Args...> func, Args... args) const;

	template<class T>
	Game::Entity* add(T *entity);

	template<class T>
	Game::Entity* add(std::shared_ptr<T>& entity);

	/** Removes an entity from all internal collections. */
	void remove(const Game::Entity& entity);
	void remove(std::shared_ptr<Game::Entity> entity);

	/** Removes all entities from this EntityGroup. */
	void clear();

	template<class T>
	size_t size() const;
	size_t size() const { return entities.size(); }

	/** Explicitly request that all expired weak_ptr's are pruned. This is done
	 *  automatically in updateAll() if this method hasn't been called since latest update.
	 *  Note that calling this method does NOT remove any killed or dead entity from the main
	 *  collection: that is only done in `updateAll`, so one can still refer to an "almost-dead"
	 *  entity (i.e. one that `isKilled()` and has `isKillInProgress() == false`) to do 
	 *  something in its last update cycle between a call to `validate()` and `updateAll()`.
	 */
	void validate();
	void updateAll();

	auto getColliding() -> std::vector<std::weak_ptr<Game::Collider>>& {
		return collidingEntities;
	}

	/** @return all entities whose tile is `tile` */
	auto getEntitiesAtTile(const sf::Vector2i& tile) const -> std::vector<std::weak_ptr<Game::Entity>>;

	inline void mtxLock() const {
#ifdef MULTITHREADED
		mutex.lock();
#endif
	}
	inline void mtxUnlock() const {
#ifdef MULTITHREADED
		mutex.unlock();
#endif
	}
};

///// Implementation /////

template<typename... Args>
void EntityGroup::apply(AppliedFunc<Game::Entity*, Args&...> func, Args&... args) {
	for (auto& e : entities)
		func(e.get(), args...);
}

template<typename... Args>
void EntityGroup::apply(AppliedFunc<const Game::Entity*, Args...> func, Args... args) const {
	for (const auto& e : entities)
		func(e.get(), args...);
}

template<typename... Args>
void EntityGroup::apply(AppliedFunc<std::weak_ptr<Game::Entity>, Args...> func, Args... args) const {
	for (const auto& e : entities)
		func(e, args...);
}

template<class T>
Game::Entity* EntityGroup::add(T *entity) {
	entity->init();
	entities.push_back(std::shared_ptr<Game::Entity>(entity));
	return _putInAux(entities.back());
}

template<class T>
Game::Entity* EntityGroup::add(std::shared_ptr<T>& entity) {
	entity->init();
	entities.push_back(entity);
	return _putInAux(entities.back());
}

template<class T>
size_t EntityGroup::size() const {
	size_t sz = 0;
	for (auto& e : entities) {
		if (dynamic_cast<T*>(e.get()) != nullptr)
			++sz;
	}
	return sz;
}

}
