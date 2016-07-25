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
// FIXME: refactor with shared/weak ptr?
class EntityGroup final : public Game::WithOrigin, private sf::NonCopyable {

	friend class Game::CollisionDetector;

	/** All the entities (owning references) */
	std::list<std::shared_ptr<Game::Entity>> entities;

	/** The colliders of entities which have one */
	std::vector<std::weak_ptr<Game::Collider>> collidingEntities;

	/** The static entities, which are always grid-aligned and cannot move,
	 *  except Temporary entities.
	 */
	std::array<Game::Entity*, Game::LEVEL_WIDTH * Game::LEVEL_HEIGHT> fixedEntities;

	/** The list of the killable entities, which ought to be removed when 
	 *  their `isKilled()` method yields true.
	 */
	std::list<std::weak_ptr<Game::Killable>> killables;

	/** The list of Killable entities who are being destroyed (those whose isKilled() is
	 *  true but isKillInProgress() is true as well).
	 */
	std::list<std::weak_ptr<Game::Killable>> dying;


	/** Removes `e`'s components from internal collections */
	void _removeFromInternal(const Game::Entity& e);

	/** Removes any killed entity from all internal collections and destroys them.
	 *  If its `isKillInProgress()` is true, puts it in `dying`
	 *  instead of immediately destroing it (it is not removed from `entities` until it's finalized)
	 */
	void _checkKilled();

	/** Removes any expired Killable in `dying`. */
	void _removeDying();

	void _setFixedAt(unsigned short x, unsigned short y, Game::Entity *e) {
		if (0 < x && x <= Game::LEVEL_WIDTH + 1 && 0 < y && y <= Game::LEVEL_HEIGHT + 1)
			fixedEntities[(y - 1) * Game::LEVEL_WIDTH + x - 1] = e;
	}

	Game::Entity* _putInAux(Game::Entity *entity);

public:
	/**
	 * Constructs the EntityGroup as the owner of its entities.
	 */
	explicit EntityGroup();

	/** Applies a void(Args...) function to all entities (ref-args version)  */
	template<typename... Args>
	void apply(AppliedFunc<Game::Entity, Args&...> func, Args&... args);

	/** Applies a void(Args...) function to all entities (const version) */
	template<typename... Args>
	void apply(AppliedFunc<const Game::Entity, Args...> func, Args... args) const;

	template<class T>
	Game::Entity* add(T *entity);

	template<class T>
	Game::Entity* add(std::shared_ptr<T>& entity);

	/** Removes an entity from all internal collections. */
	void remove(const Game::Entity& entity);

	/** Removes all entities from this EntityGroup. */
	void clear();

	void setOrigin(const sf::Vector2f& origin) override;

	template<class T>
	size_t size() const;
	size_t size() const { return entities.size(); }

	bool isFixed(const Game::Entity& entity) const;

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
Game::Entity* EntityGroup::add(T *entity) {
	entities.push_back(std::shared_ptr<Game::Entity>(entity));
	return _putInAux(entity);
}

template<class T>
Game::Entity* EntityGroup::add(std::shared_ptr<T>& entity) {
	entities.push_back(entity);
	return _putInAux(entity.get());
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
