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
 */
class EntityGroup final : public Game::WithOrigin, private sf::NonCopyable {

	friend class Game::CollisionDetector;

	/** All the entities (owning references) */
	std::list<std::shared_ptr<Game::Entity>> entities;

	/** The colliders of entities which have one */
	std::vector<std::weak_ptr<Game::Collider>> collidingEntities;

	/** The static entities, which are always grid-aligned and cannot move,
	 *  except Temporary entities.
	 */
	std::array<std::vector<std::weak_ptr<Game::Entity>>, Game::LEVEL_WIDTH * Game::LEVEL_HEIGHT> fixedEntities;

	/** The list of the killable entities, which ought to be removed when 
	 *  their `isKilled()` method yields true.
	 */
	std::list<std::weak_ptr<Game::Killable>> killables;

	/** The list of Killable entities who are being destroyed (those whose isKilled() is
	 *  true but isKillInProgress() is true as well).
	 */
	std::list<std::weak_ptr<Game::Killable>> dying;


	/** Removes any killed entity from all internal collections and destroys them.
	 *  If its `isKillInProgress()` is true, puts it in `dying`
	 *  instead of immediately destroing it (it is not removed from `entities` until it's finalized)
	 */
	void _checkKilled();

	/** Removes any expired Killable in `dying`. */
	void _removeDying();

	/** Adds a fixed entity at tile x, y. Fails silently if x or y are out of bounds. */
	void _addFixedAt(unsigned short x, unsigned short y, const std::shared_ptr<Game::Entity>& e);
	/** Removes fixed entity `e` from tile x, y. Fails silently if x or y are out of bounds or if `e` is not
	 *  at that tile.
	 */
	void _rmFixedAt(unsigned short x, unsigned short y, const Game::Entity& e);
	void _pruneFixed();

	Game::Entity* _putInAux(const std::shared_ptr<Game::Entity>& entity);

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

	/** Makes the EntityGroup flush all weak ptrs referring to `entity` from internal collections, then
	 *  readds them if necessary. Useful when a component is added or removed from `entity` after its creation.
	 */
	void refresh(const Game::Entity& entity);

	/** Removes all entities from this EntityGroup. */
	void clear();

	void setOrigin(const sf::Vector2f& origin) override;

	template<class T>
	size_t size() const;
	size_t size() const { return entities.size(); }

	void updateAll();

	/** Returns the fixed entity at tile (x, y), where x and y range from 1 to width/height.
	 *  Returns an empty vector if x or y are out of bounds.
	 */ 
	std::vector<std::reference_wrapper<Game::Entity>> getFixedAt(unsigned short x, unsigned short y) const;
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
