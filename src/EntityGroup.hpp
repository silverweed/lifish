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

namespace Game {

class CollisionDetector;

namespace {
	template<class T>
	struct identity {
		typedef T type;
	};

	template<class T, typename... Args>
	using AppliedFunc = typename identity<std::function<void(T*, Args...)>>::type;
}

/**
 * A container for Entities, providing convenient methods for operating
 * on all or a specific type of them.
 * EntityGroup is _not_ thread-safe.
 */
class EntityGroup final : public Game::WithOrigin, private sf::NonCopyable {

	friend class Game::CollisionDetector;

	/** All the entities (owning references) */
	std::list<std::unique_ptr<Game::Entity>> entities;

	/** The colliders of entities which have one */
	std::vector<Game::Collider*> collidingEntities;

	/** The static entities, which are always grid-aligned and cannot move,
	 *  except Temporary entities.
	 */
	std::array<Game::Entity*, Game::LEVEL_WIDTH * Game::LEVEL_HEIGHT> fixedEntities;

	/** The list of the temporary entities, which have a brief lifetime
	 *  and ought to be removed when their Temporary component tells they're expired.
	 */
	std::list<Game::Temporary*> temporary;

	/** The list of Killable entities who are being destroyed (those whose isKilled() is
	 *  true but isKillInProgress() is true as well).
	 */
	std::list<Game::Killable*> dying;


	/** Removes any expired temporary from both `temporary` and `entities`, and destroys them.
	 *  If an entity is Killable and its `isKillInProgress()` is true, puts it in `dying`
	 *  instead of immediately destroing it.
	 */
	void _removeExpiredTemporaries();

	/** Removes any expired Killable in `dying`. */
	void _removeDying();

	void _setFixedAt(unsigned short x, unsigned short y, Game::Entity *e) {
		if (0 < x && x <= Game::LEVEL_WIDTH + 1 && 0 < y && y <= Game::LEVEL_HEIGHT + 1)
			fixedEntities[(y - 1) * Game::LEVEL_WIDTH + x - 1] = e;
	}

public:
	/**
	 * Constructs the EntityGroup as the owner of its entities.
	 */
	explicit EntityGroup();

	/** Applies a void(Args...) function to all entities */
	template<typename... Args>
	void apply(AppliedFunc<Game::Entity, Args...> func, Args... args);

	/** Applies a void(Args...) function to all entities (ref-args version)  */
	template<typename... Args>
	void apply(AppliedFunc<Game::Entity, Args&...> func, Args&... args);

	/** Applies a void(Args...) function to all entities (const version) */
	template<typename... Args>
	void apply(AppliedFunc<const Game::Entity, Args...> func, Args... args) const;

	template<class T>
	T* add(T *entity);

	/** Removes an entity from all internal collections. */
	void remove(Game::Entity *entity);

	/** Relinquishes the ownership of `e` to the callee.
	 *  `e` is then removed from all internal collections. 
	 */
	Game::Entity* release(Game::Entity *e);

	/** Removes all entities from this EntityGroup. */
	void clear();

	void setOrigin(const sf::Vector2f& origin) override;

	template<class T>
	size_t size() const;
	size_t size() const { return entities.size(); }

	void updateAll();

	/** Returns the fixed entity at tile (x, y), where x and y range from 1 to width/height */ 
	Game::Entity* getFixedAt(unsigned short x, unsigned short y) const {
		if (0 < x && x <= Game::LEVEL_WIDTH + 1 && 0 < y && y <= Game::LEVEL_HEIGHT + 1)
			return fixedEntities[(y - 1) * Game::LEVEL_WIDTH + x - 1];
		return nullptr;
	}
};

///// Implementation /////

template<typename... Args>
void EntityGroup::apply(AppliedFunc<Game::Entity, Args...> func, Args... args) {
	for (auto& e : entities)
		func(e.get(), args...);
}

template<typename... Args>
void EntityGroup::apply(AppliedFunc<Game::Entity, Args&...> func, Args&... args) {
	for (auto& e : entities)
		func(e.get(), args...);
}

template<typename... Args>
void EntityGroup::apply(AppliedFunc<const Game::Entity, Args...> func, Args... args) const {
	for (const auto& e : entities)
		func(e.get(), args...);
}

template<class T>
T* EntityGroup::add(T *entity) {
	entities.push_back(std::unique_ptr<Game::Entity>(entity));
	entity->setOrigin(origin);

	// Put in aux collections
	auto tmp = entity->template get<Game::Temporary>();
	if (tmp != nullptr) {
		temporary.push_back(tmp);
	} else if (entity->template get<Game::Moving>() == nullptr) {
		// Put non-temporary fixed entity in fixedEntities
		const auto tile = Game::tile(entity->getPosition());
		if (getFixedAt(tile.x, tile.y) != nullptr)
			throw std::logic_error("Two fixed entities share the same tile?!");

		_setFixedAt(tile.x, tile.y, entity);
	}

	auto cld = entity->template get<Game::Collider>();
	if (cld != nullptr && !cld->isPhantom())
		collidingEntities.push_back(cld);

	return entity;
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
