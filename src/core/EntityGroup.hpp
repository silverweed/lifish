#pragma once

#include "Collider.hpp"
#include "Entity.hpp"
#include "Fixed.hpp"
#include "Killable.hpp"
#include "Moving.hpp"
#include "Temporary.hpp"
#include <SFML/System/NonCopyable.hpp>
#include <algorithm>
#include <array>
#include <functional>
#include <iterator>
#include <memory>
#include <type_traits>
#include <unordered_set>
#include <vector>

namespace lif {

class CollisionDetector;
class LevelRenderer;

/**
 * A container for Entities, providing convenient methods for operating
 * on all or a specific type of them.
 */
class EntityGroup final : private sf::NonCopyable {

	bool alreadyPrunedThisUpdate = false,
	     alreadyCheckedThisUpdate = false;

	/** All the entities (owning references) */
	std::vector<std::shared_ptr<lif::Entity>> entities;

	/** The colliders of entities which have one */
	std::vector<std::weak_ptr<lif::Collider>> collidingEntities;

	/** The vector of the killable entities, which ought to be removed when
	 *  their `isKilled()` method yields true.
	 */
	std::vector<std::weak_ptr<lif::Killable>> killables;

	/** The vector of Killable entities who are being destroyed (those whose isKilled() is
	 *  true but isKillInProgress() is true as well).
	 */
	std::vector<std::weak_ptr<lif::Killable>> dying;


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

	lif::Entity* _putInAux(lif::Entity *entity);
public:
	static constexpr bool APPLY_PROCEED = false;
	static constexpr bool APPLY_EXIT = true;

	/**
	 * Constructs the EntityGroup as the owner of its entities.
	 */
	explicit EntityGroup();

	/** Applies a function to all entities.
	 *  If the passed function returns APPLY_EXIT for an Entity,
	 *  this function will return without processing additional entities.
	 */

	template<typename F, typename...Args,
		typename std::enable_if<std::is_same<
			typename std::invoke_result<F(lif::Entity&, Args&&...)>::type, bool>::value,
				std::nullptr_t>::type = nullptr>
	void apply(const F& func, Args&&... args) {
		for (auto& e : entities)
			if (func(*e, std::forward<Args>(args)...))
				return;
	}

	/** Applies a function to all entities. */
	template<typename F, typename...Args,
		typename std::enable_if<std::is_same<
			typename std::invoke_result<F, lif::Entity&, Args&&...>::type, void>::value,
				std::nullptr_t>::type = nullptr>
	void apply(const F& func, Args&&... args) {
		for (auto& e : entities)
			func(*e, std::forward<Args>(args)...);
	}

	/** @see apply */
	template<typename F, typename...Args,
		typename std::enable_if<std::is_same<
			typename std::invoke_result<F, lif::Entity&, Args&&...>::type, bool>::value,
				std::nullptr_t>::type = nullptr>
	void apply(const F& func, Args&&... args) const {
		for (const auto& e : entities)
			if (func(*e, std::forward<Args>(args)...))
				return;
	}

	/** @see apply */
	template<typename F, typename...Args,
		typename std::enable_if<std::is_same<
			typename std::invoke_result<F, lif::Entity&, Args&&...>::type, void>::value,
				std::nullptr_t>::type = nullptr>
	void apply(const F& func, Args&&... args) const {
		for (const auto& e : entities)
			func(*e, std::forward<Args>(args)...);
	}

	/** Adds an entity to this group. */
	lif::Entity* add(lif::Entity *entity);

	/** @see add */
	template<typename T>
	T* add(std::shared_ptr<T> entity);

	/* Creates an entity of type T and adds it to this group. */
	template<typename T, typename...Args>
	T* add(Args&&... args);

	/** Removes all entities from this EntityGroup. */
	void clear();

	/** @return the number of entities of type T in this group */
	template<typename T>
	size_t size() const;
	/** @return the total number of entitiies in this group */
	size_t size() const { return entities.size(); }

	/** Explicitly request that all expired weak_ptr's are pruned. This is done
	 *  automatically in updateAll() if this method hasn't been called since latest update.
	 *  Note that calling this method does NOT remove any killed or dead entity from the main
	 *  collection: that is only done in `updateAll`, so one can still refer to an "almost-dead"
	 *  entity (i.e. one that `isKilled()` and has `isKillInProgress() == false`) to do
	 *  something in its last update cycle between a call to `validate()` and `updateAll()`.
	 */
	void validate();
	/** Explicitly request that the internal helper lists are updated. */
	void checkAll();
	/** Calls `update` for every entity in this group */
	void updateAll();

	auto getColliding() -> std::vector<std::weak_ptr<lif::Collider>>& {
		return collidingEntities;
	}

	auto getColliding() const -> const std::vector<std::weak_ptr<lif::Collider>>& {
		return collidingEntities;
	}

	/** @return all colliders intersecting `rect`.
	 *  NOTE: these pointers are only guaranteed to be valid until the next call to updateAll(), so
	 *  the caller should *not* retain them.
	 */
	auto getCollidersIntersecting(const sf::FloatRect& rect) const -> std::vector<lif::Collider*>;
};

///// Implementation /////

template<typename T>
T* EntityGroup::add(std::shared_ptr<T> entity) {
	entity->init();
	entities.emplace_back(entity);
	return static_cast<T*>(_putInAux(entities.back().get()));
}

template<typename T, typename...Args>
T* EntityGroup::add(Args&&... args) {
	entities.emplace_back(std::make_shared<T>(std::forward<Args>(args)...));
	entities.back()->init();
	return static_cast<T*>(_putInAux(entities.back().get()));
}

template<typename T>
size_t EntityGroup::size() const {
	return std::count_if(entities.begin(), entities.end(), [] (const auto& e) {
		return dynamic_cast<const T*>(e.get()) != nullptr;
	});
}

} // end namespace lif

